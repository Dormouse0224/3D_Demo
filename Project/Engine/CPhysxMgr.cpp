#include "pch.h"

#include "CPhysxMgr.h"
#include "CTimeMgr.h"
#include "CRenderMgr.h"

#include "CTransform.h"
#include "CGameObject.h"
#include "CDevice.h"
#include "CGraphicShader.h"
#include "CConstBuffer.h"
#include "CSimulationEvent.h"
#include "CCamera.h"
#include "CPhysxActor.h"


PxFilterFlags FilterShaderExample(
    PxFilterObjectAttributes attributes0, PxFilterData filterData0,
    PxFilterObjectAttributes attributes1, PxFilterData filterData1,
    PxPairFlags& pairFlags, const void* constantBlock, PxU32 constantBlockSize)
{
    // let triggers through
    if (PxFilterObjectIsTrigger(attributes0) || PxFilterObjectIsTrigger(attributes1))
    {
        pairFlags = PxPairFlag::eTRIGGER_DEFAULT;
        return PxFilterFlag::eDEFAULT;
    }
    // generate contacts for all that were not filtered above
    pairFlags = PxPairFlag::eCONTACT_DEFAULT;

    pairFlags |= PxPairFlag::eDETECT_CCD_CONTACT;

    // 충돌 시 onContact 를 호출하는 상황 설정
    //if ((filterData0.word0 & filterData1.word1) && (filterData1.word0 & filterData0.word1))
    //    pairFlags |= PxPairFlag::eNOTIFY_TOUCH_FOUND        // 충돌 시작
    //                 | PxPairFlag::eNOTIFY_TOUCH_PERSISTS    // 충돌 중
    //                 | PxPairFlag::eNOTIFY_TOUCH_LOST        // 충돌 종료
    //                 | PxPairFlag::eNOTIFY_CONTACT_POINTS;

    pairFlags |= PxPairFlag::eNOTIFY_TOUCH_FOUND        // 충돌 시작
        | PxPairFlag::eNOTIFY_TOUCH_PERSISTS    // 충돌 중
        | PxPairFlag::eNOTIFY_TOUCH_LOST        // 충돌 종료
        | PxPairFlag::eNOTIFY_CONTACT_POINTS;

    return PxFilterFlag::eDEFAULT;
}


CPhysxMgr::CPhysxMgr()
    : m_Dispatcher(nullptr)
    , m_Foundation(nullptr)
    , m_Physics(nullptr)
    , m_Scene(nullptr)
{
}

CPhysxMgr::~CPhysxMgr()
{
    if(m_Scene)
        m_Scene->release();
    if (m_Physics)
        m_Physics->release();
    if (m_Dispatcher)
        m_Dispatcher->release();
    if (m_Foundation)
        m_Foundation->release();

    delete m_EventCallback;
}

void CPhysxMgr::Init()
{
    // physx 초기화
    m_Foundation = PxCreateFoundation(PX_PHYSICS_VERSION, m_DefaultAllocatorCallback, m_DefaultErrorCallback);
    if (!m_Foundation) throw("PxCreateFoundation failed!");
    m_ToleranceScale.length = 1.f;
    m_ToleranceScale.speed = 400.f;
    m_Physics = PxCreatePhysics(PX_PHYSICS_VERSION, *m_Foundation, m_ToleranceScale);

    PxSceneDesc sceneDesc(m_Physics->getTolerancesScale());
    sceneDesc.gravity = PxVec3(0.f, -800.f, 0.f);
    m_Dispatcher = PxDefaultCpuDispatcherCreate(2);
    sceneDesc.cpuDispatcher = m_Dispatcher;
    sceneDesc.filterShader = FilterShaderExample;
    sceneDesc.flags |= PxSceneFlag::eENABLE_CCD;
    m_Scene = m_Physics->createScene(sceneDesc);

    // 디버그 렌더링 목록
    m_Scene->setVisualizationParameter(PxVisualizationParameter::eSCALE, 1.0f);
    m_Scene->setVisualizationParameter(PxVisualizationParameter::eBODY_AXES, 10.0f);
    m_Scene->setVisualizationParameter(PxVisualizationParameter::eACTOR_AXES, 10.0f);
    m_Scene->setVisualizationParameter(PxVisualizationParameter::eCOLLISION_SHAPES, 1.0f);

    // 충돌 이벤트 콜백 등록
    m_EventCallback = new CSimulationEvent;
    m_Scene->setSimulationEventCallback(m_EventCallback);

    // physx 디버그용 셰이더 컴파일
    m_Shader = new CGraphicShader;

    m_Shader->CreateVertexShader(L"HLSL\\Engine\\PhysxDebug.fx", "VS_PhysxDebug");
    m_Shader->CreatePixelShader(L"HLSL\\Engine\\PhysxDebug.fx", "PS_PhysxDebug");
    m_Shader->SetDomain(SHADER_DOMAIN::DOMAIN_DEBUG);
    m_Shader->SetRSType(RS_TYPE::CULL_NONE);
    m_Shader->SetDSType(DS_TYPE::LESS);
    m_Shader->SetBSType(BS_TYPE::ALPHABLEND);
    m_Shader->SetTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
}

void CPhysxMgr::Tick()
{
    PxU32 s = m_Scene->getNbActors(PxActorTypeFlag::eRIGID_DYNAMIC);
    PxU32 ss = m_Scene->getNbActors(PxActorTypeFlag::eRIGID_STATIC);
    // 물리 시뮬레이션 실행
    float DeltaTime = EngineDT;
    float LowerLimit = 1.f / 30.f;
    if (DeltaTime > LowerLimit)
        DeltaTime = LowerLimit;
    m_Scene->simulate(DeltaTime);
    m_Scene->fetchResults(true);


    // 시뮬레이션 후 모든 동적 강체 액터 반환, 좌표 업데이트
    PxU32 DACount = m_Scene->getNbActors(PxActorTypeFlag::eRIGID_DYNAMIC);
    vector<PxActor*> vecDynamicActor(DACount, nullptr);
    m_Scene->getActors(PxActorTypeFlag::eRIGID_DYNAMIC, vecDynamicActor.data(), vecDynamicActor.size());

    for (int i = 0; i < vecDynamicActor.size(); ++i)
    {
        CGameObject* pObject = (CGameObject*)(vecDynamicActor[i]->userData);
        PxVec3 Pos = ((PxRigidDynamic*)(vecDynamicActor[i]))->getGlobalPose().p;
        PxQuat Rot = ((PxRigidDynamic*)(vecDynamicActor[i]))->getGlobalPose().q;
        pObject->Transform()->SetRelativePos(Pos.x, Pos.y, Pos.z);
        pObject->Transform()->SetRelativeRot(Vec4(Rot.x, Rot.y, Rot.z, Rot.w));
    }
}

void CPhysxMgr::Render()
{
    PxU32 s = m_Scene->getNbActors(PxActorTypeFlag::eRIGID_DYNAMIC);
    PxU32 ss = m_Scene->getNbActors(PxActorTypeFlag::eRIGID_STATIC);
    // 디버그 렌더링 정보를 정점 정보로 치환 후, 정점 버퍼를 생성
    const PxRenderBuffer& RenderBuffer = m_Scene->getRenderBuffer();
    if (RenderBuffer.getNbLines() == 0)
        return;

    ComPtr<ID3D11Buffer> VB = nullptr;

    D3D11_BUFFER_DESC desc = {};
    desc.ByteWidth = RenderBuffer.getNbLines() * sizeof(Vtx) * 2;
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.CPUAccessFlags = 0;
    desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

    vector<Vtx> vecVtx = {};
    for (PxU32 i = 0; i < RenderBuffer.getNbLines(); i++)
    {
        const PxDebugLine& line = RenderBuffer.getLines()[i];
        Vtx vtx1, vtx2;
        vtx1.vPos = Vec3(line.pos0.x, line.pos0.y, line.pos0.z);
        vtx2.vPos = Vec3(line.pos1.x, line.pos1.y, line.pos1.z);
        vtx1.vColor = Vec4(((line.color0 >> 16) & 0xFF) / 255.f, ((line.color0 >> 8) & 0xFF) / 255.f, ((line.color0 >> 0) & 0xFF) / 255.f, ((line.color0 >> 24) & 0xFF) / 255.f);
        vtx2.vColor = Vec4(((line.color1 >> 16) & 0xFF) / 255.f, ((line.color1 >> 8) & 0xFF) / 255.f, ((line.color1 >> 0) & 0xFF) / 255.f, ((line.color1 >> 24) & 0xFF) / 255.f);
        vecVtx.push_back(vtx1);
        vecVtx.push_back(vtx2);
    }

    D3D11_SUBRESOURCE_DATA sub = {};
    sub.pSysMem = vecVtx.data();

    DEVICE->CreateBuffer(&desc, &sub, VB.GetAddressOf());

    // 셰이더 및 정점 버퍼 바인딩
    m_Shader->Binding();

    // 오브젝트의 위치를 상수버퍼로 이동시킨다.
    CConstBuffer* pTransformBuffer = CDevice::GetInst()->GetConstBuffer(CB_TYPE::TRANSFORM);

    Matrix VM = CRenderMgr::GetInst()->GetCurrentCam()->GetViewMat();
    Matrix PM = CRenderMgr::GetInst()->GetCurrentCam()->GetProjMat();
    g_Trans.matWorld = Matrix::Identity;
    g_Trans.matWV = g_Trans.matWorld * VM;
    g_Trans.matVP = VM * PM;
    g_Trans.matWVP = g_Trans.matWV * PM;

    pTransformBuffer->SetData(&g_Trans, sizeof(tTransform));
    pTransformBuffer->Binding();

    UINT stride = sizeof(Vtx);
    UINT Offset = 0;
    CONTEXT->IASetVertexBuffers(0, 1, VB.GetAddressOf(), &stride, &Offset);

    // 렌더
    CONTEXT->Draw(vecVtx.size(), 0);
}




PxRigidActor* CPhysxMgr::FindRigidBody(CGameObject* _Object)
{
    map<CGameObject*, PxRigidActor*>::iterator iter = m_mapRigidBody.find(_Object);

    if (iter == m_mapRigidBody.end())
        return nullptr;

    return iter->second;
}

CGameObject* CPhysxMgr::FindBodyOwner(PxRigidActor* _Actor)
{
    map<PxRigidActor*, CGameObject*>::iterator iter = m_mapRigidBody2.find(_Actor);

    if (iter == m_mapRigidBody2.end())
        return nullptr;

    return iter->second;
}

void CPhysxMgr::AddRigidBody(CGameObject* _Object, PxRigidActor* _Actor)
{
    PxU32 s = m_Scene->getNbActors(PxActorTypeFlag::eRIGID_DYNAMIC);
    PxU32 ss = m_Scene->getNbActors(PxActorTypeFlag::eRIGID_STATIC);
    m_Scene->addActor(*_Actor);
    m_mapRigidBody.insert(make_pair(_Object, _Actor));
    m_mapRigidBody2.insert(make_pair(_Actor, _Object));
}

void CPhysxMgr::RemoveRigidBody(CGameObject* _Object)
{
    map<CGameObject*, PxRigidActor*>::iterator iter = m_mapRigidBody.find(_Object);
    if (iter == m_mapRigidBody.end())
        return;

    PxActor* pActor = iter->second;
    m_Scene->removeActor(*pActor);
    pActor->release();
    pActor = nullptr;
    _Object->PhysxActor()->m_Body = nullptr;

    auto iter2 = m_mapRigidBody2.find(iter->second);
    m_mapRigidBody.erase(iter);
    m_mapRigidBody2.erase(iter2);
}

void CPhysxMgr::ClearScene()
{
    int NbActors = m_Scene->getNbActors(PxActorTypeFlag::eRIGID_STATIC | PxActorTypeFlag::eRIGID_DYNAMIC);
    vector<PxActor*> vecActors(NbActors);
    m_Scene->getActors(PxActorTypeFlag::eRIGID_STATIC | PxActorTypeFlag::eRIGID_DYNAMIC, vecActors.data(), NbActors);
    m_Scene->removeActors(vecActors.data(), NbActors);
}
