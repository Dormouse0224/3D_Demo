#pragma once

class CGameObject;
class CGraphicShader;
class CSimulationEvent;

//struct tActorData
//{
//	UINT		m_ID;
//	wstring		m_Name;
//};

class CPhysxMgr
	: public Singleton<CPhysxMgr>
{
	//friend class CPhysxActor;
public:
	SINGLE(CPhysxMgr);

private:
	// 초기화 데이터
	PxDefaultAllocator					m_DefaultAllocatorCallback;
	PxDefaultErrorCallback				m_DefaultErrorCallback;
	PxDefaultCpuDispatcher*				m_Dispatcher;
	PxTolerancesScale					m_ToleranceScale;

	PxFoundation*						m_Foundation;
	PxPhysics*							m_Physics;

	PxScene*							m_Scene;

	// 시뮬레이션 대상 관리 컨테이너
	map<CGameObject*, PxRigidActor*>	m_mapRigidBody;
	map<PxRigidActor*, CGameObject*>	m_mapRigidBody2;

	// 디버그용 셰이더
	AssetPtr<CGraphicShader>			m_Shader;

	// 충돌 이벤트 처리 객체
	CSimulationEvent*					m_EventCallback;

public:
	void Init();
	void Tick();
	void Render();

	PxPhysics* GetPhysics() { return m_Physics; }
	PxScene* GetScene() { return m_Scene; }

	PxRigidActor* FindRigidBody(CGameObject* _Object);
	CGameObject* FindBodyOwner(PxRigidActor* _Actor);

	/// <summary>
	/// 시뮬레이션에 해당 오브젝트를 키값으로 하여 강체 액터를 등록합니다.
	/// </summary>
	/// <param name="_Object"></param>
	/// <param name="_Actor"></param>
	void AddRigidBody(CGameObject* _Object, PxRigidActor* _Actor);

	/// <summary>
	/// 시뮬레이션에 해당 오브젝트로 등록된 강체 Actor를 찾아 제거합니다.
	/// </summary>
	/// <param name="_Object"></param>
	void RemoveRigidBody(CGameObject* _Object);

	void ClearScene();
};

