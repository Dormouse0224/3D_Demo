#include "pch.h"
#include "CPhysxActor.h"
#include "CTransform.h"

#include "CLevelMgr.h"

#include "CLevel.h"
#include "CLayer.h"

wstring LOCK_FLAG_WSTR[] =
{
    L"LINEAR_X",
    L"LINEAR_Y",
    L"LINEAR_Z",
    L"ANGULAR_X",
    L"ANGULAR_Y",
    L"ANGULAR_Z",
};

CPhysxActor::CPhysxActor()
    : CComponent(COMPONENT_TYPE::PHYSXACTOR)
    , m_Body(nullptr)
    , m_Type(RIGID_TYPE::NONE)
    , m_LockFlag(0)
    , m_Gravity(false)
    , m_Density(1.f)
    , m_InitVel(0)
    , m_vecDesc{}
    , m_vecScale{}
    , m_vecOffset{}
    , m_DelayedInit(false)
    , m_ContactBeginInst(nullptr)
    , m_ContactTickInst(nullptr)
    , m_ContactEndInst(nullptr)
    , m_ContactBegin(nullptr)
    , m_ContactTick(nullptr)
    , m_ContactEnd(nullptr)
{

}

CPhysxActor::CPhysxActor(const CPhysxActor& _Other)
    : CComponent(_Other)
    , m_Body(nullptr)
    , m_Type(_Other.m_Type)
    , m_LockFlag(_Other.m_LockFlag)
    , m_Gravity(_Other.m_Gravity)
    , m_Density(_Other.m_Density)
    , m_InitVel(_Other.m_InitVel)
    , m_vecDesc{ _Other.m_vecDesc }
    , m_vecScale{ _Other.m_vecScale }
    , m_vecOffset{ _Other.m_vecOffset }
    , m_DelayedInit(true)
    , m_ContactBeginInst(nullptr)
    , m_ContactTickInst(nullptr)
    , m_ContactEndInst(nullptr)
    , m_ContactBegin(nullptr)
    , m_ContactTick(nullptr)
    , m_ContactEnd(nullptr)
{
   
}

CPhysxActor::~CPhysxActor()
{
    CPhysxMgr::GetInst()->RemoveRigidBody(GetOwner());
}

void CPhysxActor::FinalTick()
{
    if (m_DelayedInit)
    {
        m_DelayedInit = false;
        // m_Body 세팅
        SetRigidBody(m_Type, m_LockFlag, m_Gravity, m_Density, m_InitVel);

        // body 에 충돌체 추가
        for (int i = 0; i < m_vecDesc.size(); ++i)
            AttachCollider(m_vecDesc[i], m_vecScale[i], m_vecOffset[i]);
    }
}

PxVec3 CPhysxActor::GetLinearVelocity()
{
    if (m_Type == RIGID_TYPE::DYNAMIC)
        return static_cast<PxRigidDynamic*>(m_Body)->getLinearVelocity();

    return PxVec3(0);
}

void CPhysxActor::SetRigidBody(RIGID_TYPE _Type, UINT _LockFlag, bool _Gravity, float _Density, PxVec3 _InitVel)
{
    m_Type = _Type;
    m_LockFlag = _LockFlag;
    m_Gravity = _Gravity;
    m_Density = _Density;

    if(m_Type == RIGID_TYPE::NONE || GetOwner() == nullptr || GetOwner()->Transform() == nullptr)
        return;

    // 오브젝트 좌표 가져오기
    PxTransform Trans(PxVec3(GetOwner()->Transform()->GetRelativePos().x, GetOwner()->Transform()->GetRelativePos().y, GetOwner()->Transform()->GetRelativePos().z));

    if (_Type == RIGID_TYPE::DYNAMIC || _Type == RIGID_TYPE::KINEMATIC)
    {
        // 강체 생성
        PxRigidDynamic* pBody = CPhysxMgr::GetInst()->GetPhysics()->createRigidDynamic(Trans);
        m_Body = pBody;
        pBody->userData = GetOwner();   // 사용자 정의 데이터 영역에 컴포넌트를 소유한 오브젝트 주소를 저장
        pBody->setRigidDynamicLockFlags((PxRigidDynamicLockFlags)_LockFlag);  // 축 이동 및 회전 잠금 플래그
        pBody->setRigidBodyFlag(PxRigidBodyFlag::eENABLE_CCD, true);
        pBody->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, !_Gravity);  // 중력 적용 옵션
        pBody->setLinearVelocity(_InitVel); // 강체 적용 시 초기 속도

        if (_Type == RIGID_TYPE::KINEMATIC)
            pBody->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, true);

        // 씬에 액터 등록
        CPhysxMgr::GetInst()->AddRigidBody(GetOwner(), pBody);
    }
    else if (_Type == RIGID_TYPE::STATIC)
    {
        // 강체 생성
        PxRigidStatic* pBody = CPhysxMgr::GetInst()->GetPhysics()->createRigidStatic(Trans);
        m_Body = pBody;
        pBody->userData = GetOwner();

        // 씬에 액터 등록
        CPhysxMgr::GetInst()->AddRigidBody(GetOwner(), pBody);
    }

}

void CPhysxActor::SetRigidType(RIGID_TYPE _Type)
{
    if (m_Type == _Type)
        return;

    if ((m_Type == RIGID_TYPE::KINEMATIC && _Type == RIGID_TYPE::DYNAMIC)
        || (m_Type == RIGID_TYPE::DYNAMIC && _Type == RIGID_TYPE::KINEMATIC))
    {
        // Dynamic <-> Kinematic 인 경우 플래그만 토글시킨다
        PxRigidDynamic* pBody = (PxRigidDynamic*)m_Body;
        pBody->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, !(pBody->getRigidBodyFlags() & PxRigidBodyFlag::eKINEMATIC));
        m_Type = _Type;
    }
    else if (_Type == RIGID_TYPE::NONE)
    {
        // None 타입으로 변경 시 기존 강체를 제거한다.
        CPhysxMgr::GetInst()->RemoveRigidBody(GetOwner());
        m_vecShape.clear();
    }
    else if (m_Type == RIGID_TYPE::NONE)
    {
        // None 타입에서 새로 생성하는 경우 강체를 만든다.
        SetRigidBody(_Type);

        for (int i = 0; i < m_vecShape.size(); ++i)
        {
            AttachCollider(m_vecDesc[i], m_vecScale[i], m_vecOffset[i]);
        }
    }
    else
    {
        // 둘 중 하나가 Static 인 경우 기존 강체를 제거하고 신규 강체로 새로 등록한다
        CPhysxMgr::GetInst()->RemoveRigidBody(GetOwner());
        m_vecShape.clear();
        SetRigidBody(_Type);

        for (int i = 0; i < m_vecDesc.size(); ++i)
        {
            AttachCollider(m_vecDesc[i], m_vecScale[i], m_vecOffset[i]);
        }
    }
}

void CPhysxActor::SetGravity(bool _Gravity)
{
    // 다이나믹 액터가 아니면 즉시 리턴
    if (PxConcreteType::eRIGID_DYNAMIC != m_Body->getConcreteType())
        return;

    m_Gravity = _Gravity;
    PxRigidDynamic* pBody = (PxRigidDynamic*)m_Body;
    pBody->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, !_Gravity);
}

void CPhysxActor::SetDensity(float _Density)
{
    m_Density = _Density;
    if (m_Body->getNbShapes() > 0 && PxConcreteType::eRIGID_DYNAMIC == m_Body->getConcreteType())
    {
        PxRigidBodyExt::updateMassAndInertia(*(PxRigidBody*)m_Body, m_Density);
    }
}

void CPhysxActor::SetColliderDesc(int _Idx, COLLIDER_DESC _Data)
{
    if (_Idx < m_vecShape.size())
    {
        m_vecDesc[_Idx] = _Data;

        m_vecShape[_Idx]->setFlags(_Data.ShapeFlag);

        PxFilterData filterData;
        filterData.word0 = _Data.FilterLayer_Self;
        filterData.word1 = _Data.FilterLayer_Other;
        if (_Data.ShapeFlag & PxShapeFlag::eSIMULATION_SHAPE)
            m_vecShape[_Idx]->setSimulationFilterData(filterData);
        if (_Data.ShapeFlag & PxShapeFlag::eSCENE_QUERY_SHAPE)
            m_vecShape[_Idx]->setQueryFilterData(filterData);


        PxMaterial* pMaterial;
        m_vecShape[_Idx]->getMaterials(&pMaterial, 1);
        if (pMaterial)
        {
            pMaterial->setStaticFriction(_Data.StaticFriction);
            pMaterial->setDynamicFriction(_Data.DynamicFriction);
            pMaterial->setRestitution(_Data.Restitution);
        }
    }
}

void CPhysxActor::SetColliderOffset(int _Idx, PxVec3 _Data)
{
    if (_Idx < m_vecShape.size())
    {
        m_vecOffset[_Idx] = _Data;
        m_Body->detachShape(*m_vecShape[_Idx]);
        m_vecShape[_Idx]->setLocalPose(PxTransform(_Data));
        m_Body->attachShape(*m_vecShape[_Idx]);
    }
}

void CPhysxActor::SetLinearVelocity(PxVec3 _Vel)
{
    if (m_Type == RIGID_TYPE::DYNAMIC)
    {
        if (m_Body)
            static_cast<PxRigidDynamic*>(m_Body)->setLinearVelocity(_Vel);
        else if (!m_Body && m_DelayedInit)
            m_InitVel = _Vel;
    }
}

PxShape* CPhysxActor::AddCollider(COLLIDER_DESC _Desc, PxVec3 _Scale, PxVec3 _Offset)
{
    m_vecDesc.push_back(_Desc);
    m_vecScale.push_back(_Scale);
    m_vecOffset.push_back(_Offset);

    return AttachCollider(_Desc, _Scale, _Offset);
}

void CPhysxActor::DeleteCollider(int _Idx)
{
    if (_Idx < m_vecShape.size())
    {
        m_vecDesc.erase(m_vecDesc.begin() + _Idx);
        m_vecScale.erase(m_vecScale.begin() + _Idx);
        m_vecOffset.erase(m_vecOffset.begin() + _Idx);

        m_Body->detachShape(*m_vecShape[_Idx]);
        m_vecShape[_Idx]->release();
        m_vecShape.erase(m_vecShape.begin() + _Idx);
    }

}

void CPhysxActor::DeleteCollider(PxShape* _Shape)
{
    for (int i = 0; i < m_vecShape.size(); ++i)
    {
        if (m_vecShape[i] == _Shape)
        {
            m_vecDesc.erase(m_vecDesc.begin() + i);
            m_vecScale.erase(m_vecScale.begin() + i);
            m_vecOffset.erase(m_vecOffset.begin() + i);

            m_Body->detachShape(*m_vecShape[i]);
            m_vecShape[i]->release();
            m_vecShape.erase(m_vecShape.begin() + i);
        }
    }
}

void CPhysxActor::DeleteAllCollider()
{
    for (int i = 0; i < m_vecShape.size(); ++i)
    {
        DeleteCollider(i);
    }

}

void CPhysxActor::UpdatePosition(Vec3 _Pos)
{
    // Scene 에 등록된 강체가 존재하는 경우, 해당 강체의 Scene 에서의 위치를 업데이트
    if (m_Body)
    {
        PxTransform Trans = PxTransform(_Pos.x, _Pos.y, _Pos.z, m_Body->getGlobalPose().q);
        if (m_Type == RIGID_TYPE::KINEMATIC)
        {
            static_cast<PxRigidDynamic*>(m_Body)->setKinematicTarget(Trans);
        }
        else
        {
            m_Body->setGlobalPose(Trans);
        }
        
    }
}

void CPhysxActor::UpdateRotation(Vec4 _RotQuat)
{
    // Scene 에 등록된 강체가 존재하는 경우, 해당 강체의 Scene 에서의 방향을 업데이트
    if (m_Body)
    {
        PxQuat Quat(_RotQuat.x, _RotQuat.y, _RotQuat.z, _RotQuat.w);
        PxTransform Trans = PxTransform(m_Body->getGlobalPose().p, Quat);
        if (m_Type == RIGID_TYPE::KINEMATIC)
        {
            static_cast<PxRigidDynamic*>(m_Body)->setKinematicTarget(Trans);
        }
        else
        {
            m_Body->setGlobalPose(Trans);
        }
    }
}

void CPhysxActor::CkeckLockFlag(LOCK_FLAG _Flag)
{
    // 다이나믹 액터가 아니면 즉시 리턴
    if (PxConcreteType::eRIGID_DYNAMIC != m_Body->getConcreteType())
        return;

    // 비트플래그 토글
    m_LockFlag = m_LockFlag ^ _Flag;
    PxRigidDynamic* pBody = (PxRigidDynamic*)m_Body;
    pBody->setRigidDynamicLockFlags(static_cast<PxRigidDynamicLockFlags>(m_LockFlag));
}

void CPhysxActor::ContactBegin(CGameObject* _Other, const PxContactPair pair)
{
    m_setContactObj.insert(_Other);
    if (m_ContactBeginInst != nullptr && m_ContactBegin != nullptr)
        (m_ContactBeginInst->*m_ContactBegin)(_Other, pair);
}

void CPhysxActor::ContactTick(CGameObject* _Other, const PxContactPair pair)
{
    if (m_ContactTickInst != nullptr && m_ContactTick != nullptr)
        (m_ContactTickInst->*m_ContactTick)(_Other, pair);
}

void CPhysxActor::ContactEnd(CGameObject* _Other, const PxContactPair pair)
{
    m_setContactObj.erase(_Other);
    if (m_ContactEndInst != nullptr && m_ContactEnd != nullptr)
        (m_ContactEndInst->*m_ContactEnd)(_Other, pair);
}

void CPhysxActor::TriggerBegin(CGameObject* _Other)
{
    m_setContactObj.insert(_Other);
    if (m_TriggerBeginInst != nullptr && m_TriggerBegin != nullptr)
        (m_TriggerBeginInst->*m_TriggerBegin)(_Other);
}

void CPhysxActor::TriggerEnd(CGameObject* _Other)
{
    m_setContactObj.erase(_Other);
    if (m_TriggerEndInst != nullptr && m_TriggerEnd != nullptr)
        (m_TriggerEndInst->*m_TriggerEnd)(_Other);
}

int CPhysxActor::Load(fstream& _Stream)
{
    if (!_Stream.is_open())
        return E_FAIL;

    // RigidBody 작성을 위해 필요한 데이터 불러오기
    _Stream.read(reinterpret_cast<char*>(&m_Type), sizeof(RIGID_TYPE));
    _Stream.read(reinterpret_cast<char*>(&m_LockFlag), sizeof(UINT));
    _Stream.read(reinterpret_cast<char*>(&m_Gravity), sizeof(bool));
    _Stream.read(reinterpret_cast<char*>(&m_Density), sizeof(float));


    // RigidBody 의 Collider 정보 불러오기
    int count = 0;
    _Stream.read(reinterpret_cast<char*>(&count), sizeof(int));
    m_vecDesc.resize(count);
    m_vecScale.resize(count);
    m_vecOffset.resize(count);
    for (int i = 0; i < count; ++i)
    {
        _Stream.read(reinterpret_cast<char*>(&m_vecDesc[i]), sizeof(COLLIDER_DESC));
        _Stream.read(reinterpret_cast<char*>(&m_vecScale[i]), sizeof(PxVec3));
        _Stream.read(reinterpret_cast<char*>(&m_vecOffset[i]), sizeof(PxVec3));
    }
    
    m_DelayedInit = true;

    return S_OK;
}

int CPhysxActor::Save(fstream& _Stream)
{
    // m_Body 는 데이터를 통해 로드 시 작성된다.
    if (!_Stream.is_open())
        return E_FAIL;
    
    // RigidBody 작성을 위해 필요한 데이터 저장
    _Stream.write(reinterpret_cast<char*>(&m_Type), sizeof(RIGID_TYPE));
    _Stream.write(reinterpret_cast<char*>(&m_LockFlag), sizeof(UINT));
    _Stream.write(reinterpret_cast<char*>(&m_Gravity), sizeof(bool));
    _Stream.write(reinterpret_cast<char*>(&m_Density), sizeof(float));

    // RigidBody 의 Collider 정보 저장
    int count = m_vecDesc.size();
    _Stream.write(reinterpret_cast<char*>(&count), sizeof(int));
    for (int i = 0; i < count; ++i)
    {
        _Stream.write(reinterpret_cast<char*>(&m_vecDesc[i]), sizeof(COLLIDER_DESC));
        _Stream.write(reinterpret_cast<char*>(&m_vecScale[i]), sizeof(PxVec3));
        _Stream.write(reinterpret_cast<char*>(&m_vecOffset[i]), sizeof(PxVec3));
    }

    return S_OK;
}

PxShape* CPhysxActor::AttachCollider(COLLIDER_DESC _Desc, PxVec3 _Scale, PxVec3 _Offset)
{
    // 마찰계수, 탄성계수, 모양 설정
    PxMaterial* pMaterial = CPhysxMgr::GetInst()->GetPhysics()->createMaterial(_Desc.StaticFriction, _Desc.DynamicFriction, _Desc.Restitution);
    pMaterial->setRestitutionCombineMode(PxCombineMode::eMIN);
    PxShape* shape = CPhysxMgr::GetInst()->GetPhysics()->createShape(PxBoxGeometry(_Scale), *pMaterial);
    shape->setFlags(_Desc.ShapeFlag);
    shape->setFlag(PxShapeFlag::eVISUALIZATION, true);

    // 충돌 필터 레이어 설정
    PxFilterData filterData;
    filterData.word0 = _Desc.FilterLayer_Self;      // 본인이 속한 레이어
    filterData.word1 = _Desc.FilterLayer_Other;     // 충돌 검사를 할 레이어
    if (_Desc.ShapeFlag & PxShapeFlag::eSIMULATION_SHAPE)
        shape->setSimulationFilterData(filterData);
    if (_Desc.ShapeFlag & PxShapeFlag::eSCENE_QUERY_SHAPE)
        shape->setQueryFilterData(filterData);

    // 오프셋 설정
    shape->setLocalPose(PxTransform(_Offset));

    // 강체에 충돌체 추가, 관리 벡터에 등록
    m_Body->attachShape(*shape);
    m_vecShape.push_back(shape);

    // 동적 액터인 경우 강체 형태와 밀도에 따라 질량을 업데이트
    if (PxConcreteType::eRIGID_DYNAMIC == m_Body->getConcreteType())
    {
        PxRigidBodyExt::updateMassAndInertia(*(PxRigidBody*)m_Body, m_Density);
    }

    return shape;
}
