#include "pch.h"
#include "CParticleRender.h"

#include "CAssetMgr.h"
#include "CStructuredBuffer.h"
#include "CTransform.h"
#include "CTimeMgr.h"
#include "CPhysxActor.h"
#include "CPhysxMgr.h"

CParticleRender::CParticleRender()
	: CRenderComponent(COMPONENT_TYPE::PARTICLERENDER)
	, m_ParticleBuffer(nullptr)
	, m_SpawnBuffer(nullptr)
	, m_ModuleBuffer(nullptr)
	, m_TickCS(nullptr)
	, m_MaxParticle(1000)
	, m_Module{}
	, m_AccTime(0.f)
	, m_SpawnCount()
	, m_Active(false)
	, m_Gravity(true)
	, m_ParticleTex(nullptr)
	, m_PrevSpaceType(m_Module.SpaceType)
{
	m_ParticleBuffer = new CStructuredBuffer;
	m_ParticleBuffer->Create(sizeof(tParticle), m_MaxParticle, SB_TYPE::SRV_UAV, true);

	m_SpawnBuffer = new CStructuredBuffer;
	m_SpawnBuffer->Create(sizeof(tSpawnCount), 1, SB_TYPE::SRV_UAV, true);

	m_ModuleBuffer = new CStructuredBuffer;
	m_ModuleBuffer->Create(sizeof(tParticleModule), 1, SB_TYPE::SRV_ONLY, true);


	CreateMtrl();

	// 파티클 Tick 컴퓨트 쉐이더
	m_TickCS = CAssetMgr::GetInst()->Load<CParticleTickCS>(L"EA_ParticleTickCS", true).Get();
}

CParticleRender::CParticleRender(const CParticleRender& _Other)
	: CRenderComponent(_Other)
	, m_ParticleBuffer(nullptr)
	, m_SpawnBuffer(nullptr)
	, m_ModuleBuffer(nullptr)
	, m_TickCS(_Other.m_TickCS)
	, m_MaxParticle(_Other.m_MaxParticle)
	, m_Module(_Other.m_Module)
	, m_AccTime(0.f)
	, m_SpawnCount(_Other.m_SpawnCount)
	, m_Active(_Other.m_Active)
	, m_Gravity(_Other.m_Gravity)
	, m_ParticleTex(_Other.m_ParticleTex)
	, m_PrevSpaceType(m_Module.SpaceType)
{
	m_ParticleBuffer = new CStructuredBuffer;
	m_ParticleBuffer->Create(sizeof(tParticle), m_MaxParticle, SB_TYPE::SRV_UAV, true);

	m_SpawnBuffer = new CStructuredBuffer;
	m_SpawnBuffer->Create(sizeof(tSpawnCount), 1, SB_TYPE::SRV_UAV, true);

	m_ModuleBuffer = new CStructuredBuffer;
	m_ModuleBuffer->Create(sizeof(tParticleModule), 1, SB_TYPE::SRV_ONLY, true);

}

CParticleRender::~CParticleRender()
{
	if (nullptr != m_ParticleBuffer)
		delete m_ParticleBuffer;

	if (nullptr != m_SpawnBuffer)
		delete m_SpawnBuffer;

	if (nullptr != m_ModuleBuffer)
		delete m_ModuleBuffer;

	if (!m_mapParticleObj.empty())
	{
		for (map<UINT, CGameObject*>::iterator iter = m_mapParticleObj.begin(); iter != m_mapParticleObj.end();)
		{
			delete iter->second;
			iter = m_mapParticleObj.erase(iter);
		}
	}
}

void CParticleRender::FinalTick()
{
	m_Module.ObjectWorldPos = Transform()->GetWorldTrans();
	// 최대 파티클 수 변경되었을 때 파티클 구조화 버퍼 재할당
	if (m_ParticleBuffer->GetElementCount() != m_MaxParticle)
	{
		delete m_ParticleBuffer;
		m_ParticleBuffer = new CStructuredBuffer;
		m_ParticleBuffer->Create(sizeof(tParticle), m_MaxParticle, SB_TYPE::SRV_UAV, true);
	}

	// Space Type 변경 확인
	if (m_Module.SpaceType != m_PrevSpaceType)
	{
		if (m_PrevSpaceType)
		{
			// 이전에 World 인 경우
			for (const auto& pair : m_mapParticleObj)
				delete pair.second;

		}
	}
	m_PrevSpaceType = m_Module.SpaceType;

	// 이번 프레임 파티클 활성화 개수 계산
	CalcSpawnCount();

	m_ModuleBuffer->SetData(&m_Module);

	m_TickCS->SetSpawnBuffer(m_SpawnBuffer);
	m_TickCS->SetParticleBuffer(m_ParticleBuffer);
	m_TickCS->SetModuleBuffer(m_ModuleBuffer);
	m_TickCS->SetNoiseTex(CAssetMgr::GetInst()->Load<CTexture2D>(L"Texture2D\\Noise\\noise_03.jpg"));

	// 파티클 컴퓨트 셰이더 Tick 수행
	m_TickCS->Execute();

	vector<tParticle> vecParticle = {};
	vecParticle.resize(m_ParticleBuffer->GetElementCount());
	m_ParticleBuffer->GetData(vecParticle.data());
	for (int i = 0; i < vecParticle.size(); ++i)
	{
		if (m_Module.SpaceType == 1)
		{
			// World 타입 파티클의 경우 활성화 시 강체를 추가
			if (vecParticle[i].Active == true && vecParticle[i].PrevActive == false)
			{
				CGameObject* pParticle = new CGameObject;
				pParticle->AddComponent(new CTransform);
				pParticle->AddComponent(new CPhysxActor);
				
				pParticle->Transform()->SetRelativePos(vecParticle[i].LocalPos.x, vecParticle[i].LocalPos.y, vecParticle[i].LocalPos.z - 3);
				pParticle->Transform()->SetRelativeScale(10.f, 10.f, 1.f);

				PxVec3 InitVel(vecParticle[i].Velocity.x, vecParticle[i].Velocity.y, vecParticle[i].Velocity.z);
				pParticle->PhysxActor()->SetRigidBody(RIGID_TYPE::DYNAMIC, LOCK_FLAG::LINEAR_Z, m_Gravity, 0.01f, InitVel);
				COLLIDER_DESC desc;
				desc.Restitution - 0.f;
				desc.ShapeFlag = PxShapeFlag::eSIMULATION_SHAPE;
				desc.FilterLayer_Self = COLLISION_LAYER::ePARTICLE;
				desc.FilterLayer_Other = COLLISION_LAYER::eLANDSCAPE;
				pParticle->PhysxActor()->AddCollider(desc, PxVec3(1.f, 1.f, 1.f), PxVec3(0.f, 0.f, 0.f));

				vecParticle[i].EntityID = pParticle->GetID();
				m_mapParticleObj.insert(make_pair(pParticle->GetID(), pParticle));
			}
			else if (vecParticle[i].Active == false && vecParticle[i].PrevActive == true)
			{
				map<UINT, CGameObject*>::iterator iter = m_mapParticleObj.find(vecParticle[i].EntityID);
				if (iter == m_mapParticleObj.end())
					continue;
				CGameObject* pParticle = iter->second;
				if (pParticle)
				{
					delete pParticle;
					pParticle = nullptr;
				}
				m_mapParticleObj.erase(iter);
			}
		}
	}
	m_ParticleBuffer->SetData(vecParticle.data());
}

void CParticleRender::Render()
{
	Transform()->Binding();

	// 파티클 위치값를 오브젝트 시뮬레이션 결과와 동기화
	vector<tParticle> vecParticle(m_ParticleBuffer->GetElementCount());
	m_ParticleBuffer->GetData(vecParticle.data());
	for (int i = 0; i < vecParticle.size(); ++i)
	{
		map<UINT, CGameObject*>::iterator iter = m_mapParticleObj.find(vecParticle[i].EntityID);
		if (iter != m_mapParticleObj.end())
		{
			vecParticle[i].LocalPos = iter->second->Transform()->GetRelativePos();
		}
	}
	m_ParticleBuffer->SetData(vecParticle.data());



	// 파티클 버퍼 바인딩
	m_ParticleBuffer->Binding(20);
	// 모듈 버퍼 바인딩
	m_ModuleBuffer->Binding(21);

	GetMaterial()->SetTexParam(TEX_0, m_ParticleTex);
	GetMaterial()->SetScalarParam(VEC4_0, Transform()->GetWorldTrans());

	// 재질 및 쉐이더 바인딩
	GetMaterial()->Binding();

	// 파티클 인스턴싱 렌더링
	GetMesh()->Render_Particle(m_MaxParticle);

	// 파티클 버퍼 Unbind
	m_ParticleBuffer->Unbind(20);
	// 모듈버퍼 Unbind
	m_ModuleBuffer->Unbind(21);
}

int CParticleRender::Load(fstream& _Stream)
{
	if (FAILED(CRenderComponent::RenderCom_Save(_Stream)))
		return E_FAIL;


	_Stream.read(reinterpret_cast<char*>(&m_MaxParticle), sizeof(UINT));
	_Stream.read(reinterpret_cast<char*>(&m_Module), sizeof(tParticleModule));
	_Stream.read(reinterpret_cast<char*>(&m_SpawnCount), sizeof(int));
	_Stream.read(reinterpret_cast<char*>(&m_Active), sizeof(bool));
	_Stream.read(reinterpret_cast<char*>(&m_Gravity), sizeof(bool));

	std::wstring ParticleTexName = {};
	int size = 0;
	_Stream.read(reinterpret_cast<char*>(&size), sizeof(int));
	ParticleTexName.resize(size);
	_Stream.read(reinterpret_cast<char*>(ParticleTexName.data()), sizeof(wchar_t) * size);
	m_ParticleTex = CAssetMgr::GetInst()->Load<CTexture2D>(ParticleTexName);

	return S_OK;
}

int CParticleRender::Save(fstream& _Stream)
{
	// m_ParticleBuffer, m_SpawnBuffer, m_ModuleBuffer 는 프레임마다 계산됨
	// m_TickCS 는 객체 생성 시 자동으로 로드됨
	// m_mapParticleObj 는 World 공간계 파티클 물리 시뮬레이션 시 자동으로 추가 또는 제거됨

	if (FAILED(CRenderComponent::RenderCom_Save(_Stream)))
		return E_FAIL;


	_Stream.write(reinterpret_cast<char*>(&m_MaxParticle), sizeof(UINT));
	_Stream.write(reinterpret_cast<char*>(&m_Module), sizeof(tParticleModule));
	_Stream.write(reinterpret_cast<char*>(&m_SpawnCount), sizeof(int));
	_Stream.write(reinterpret_cast<char*>(&m_Active), sizeof(bool));
	_Stream.write(reinterpret_cast<char*>(&m_Gravity), sizeof(bool));

	std::wstring ParticleTexName = {};
	if (m_ParticleTex.Get())
		ParticleTexName = m_ParticleTex->GetName();
	int size = ParticleTexName.size();
	_Stream.write(reinterpret_cast<char*>(&size), sizeof(int));
	_Stream.write(reinterpret_cast<char*>(ParticleTexName.data()), sizeof(wchar_t) * size);

	return S_OK;

}

void CParticleRender::CreateMtrl()
{
	if (nullptr == GetMesh())
		SetMesh(CAssetMgr::GetInst()->Load<CMesh>(L"EA_PointMesh", true));

	if (nullptr != GetMaterial())
		return;

	AssetPtr<CMaterial> pMtrl = CAssetMgr::GetInst()->Load<CMaterial>(L"ParticleMtrl", true);
	if (nullptr != pMtrl)
	{
		SetMaterial(pMtrl);
		return;
	}

	// 파티클 렌더링 쉐이더 및 재질
	AssetPtr<CGraphicShader> pShader = new CGraphicShader;
	pShader->SetName(L"ParticleRenderShader");

	pShader->CreateVertexShader(L"HLSL\\Engine\\particle.fx", "VS_Particle");
	pShader->CreateGeometryShader(L"HLSL\\Engine\\particle.fx", "GS_Particle");
	pShader->CreatePixelShader(L"HLSL\\Engine\\particle.fx", "PS_Particle");
	pShader->SetDomain(SHADER_DOMAIN::DOMAIN_TRANSPARENT);
	pShader->SetBSType(BS_TYPE::ALPHABLEND);
	pShader->SetRSType(RS_TYPE::CULL_NONE);
	pShader->SetDSType(DS_TYPE::NO_WRITE);
	pShader->SetTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
	CAssetMgr::GetInst()->AddAsset(pShader->GetName(), pShader.Get());

	pMtrl = new CMaterial;
	pMtrl->SetName(L"ParticleMtrl");
	pMtrl->SetShader(pShader);
	CAssetMgr::GetInst()->AddAsset(pMtrl->GetName(), pMtrl.Get());

	// 머티리얼 설정
	SetMaterial(pMtrl);
}

void CParticleRender::CalcSpawnCount()
{
	if (m_Active)
	{
		if (m_Module.SpawnRate == 0)
		{
			// 일회성 스폰
			m_Active = false;
			tSpawnCount count = {};
			count.SpawnCount = m_SpawnCount;
			m_SpawnBuffer->SetData(&count);
		}
		else
		{
			// 주기 스폰
			m_AccTime += DT;
			float Term = 1.f / m_Module.SpawnRate;
			if (Term < m_AccTime)
			{
				m_AccTime -= Term;
				tSpawnCount count = {};
				count.SpawnCount = m_SpawnCount;
				m_SpawnBuffer->SetData(&count);
			}
		}
	}
	else
	{
		tSpawnCount count = {};
		count.SpawnCount = 0;
		m_SpawnBuffer->SetData(&count);
	}
}
