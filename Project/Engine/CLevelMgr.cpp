#include "pch.h"
#include "CLevelMgr.h"

#include "CRenderMgr.h"
#include "CPhysxMgr.h"
#include "CTaskMgr.h"

#include "CLevel.h"
#include "CGameObject.h"
#include "CLayer.h"

CLevelMgr::CLevelMgr()
	: m_CurLevel(nullptr)
	, m_PlayStartLevel(nullptr)
{
}

CLevelMgr::~CLevelMgr()
{
	delete m_CurLevel;
	m_CurLevel = nullptr;

	delete m_PlayStartLevel;
	m_PlayStartLevel = nullptr;

	for (const auto& pair : m_mapLevelList)
	{
		delete pair.second;
	}
	m_mapLevelList.clear();
}

void CLevelMgr::Init()
{

}

void CLevelMgr::Progress()
{
	if (m_LevelModified)
		m_LevelModified = false;
	if (m_LevelModifyChecker)
	{
		m_LevelModifyChecker = false;
		m_LevelModified = true;
	}

	if (m_CurLevel == nullptr)
		return;

	// =====
	// Tick
	// =====
	m_CurLevel->Tick();

	m_CurLevel->Deregister();
	m_CurLevel->Register();

	m_CurLevel->FinalTick();

}

void CLevelMgr::SetPlayStartLevel(CLevel* _Level)
{
	if (m_PlayStartLevel)
		delete m_PlayStartLevel;
	m_PlayStartLevel = _Level;
}

CLevel* CLevelMgr::FindLevel(wstring _RelativePath)
{
	// 레벨을 저장해둔 컨테이너에서 검색, 찾으면 해당 레벨의 클론을 반환, 못찾으면 파일로부터 로드 시도
	const auto iter = m_mapLevelList.find(_RelativePath);
	if (iter == m_mapLevelList.end())
	{
		CLevel* pLevel = new CLevel;
		if (FAILED(pLevel->Load(_RelativePath)))
		{
			delete pLevel;
		}
		m_mapLevelList.insert(make_pair(_RelativePath, pLevel));
		return pLevel->Clone();
	}
	else
		return iter->second->Clone();
}

void CLevelMgr::DeleteLevel(wstring _RelativePath)
{
	const auto iter = m_mapLevelList.find(_RelativePath);
	if (iter == m_mapLevelList.end())
		return;
	else
	{
		delete iter->second;
		m_mapLevelList.erase(iter);
	}
}

CGameObject* CLevelMgr::AddGameObject(wstring _Name, LAYER _Layer)
{
	CGameObject* pObject = new CGameObject;
	pObject->SetName(_Name);
	m_CurLevel->AddGameObject(pObject, static_cast<int>(_Layer), true);
	m_LevelModified = true;

	return pObject;
}

void CLevelMgr::ChangeLevel(CLevel* _NextLevel, bool _Play)
{
	// 레벨이 변경되면서, Render Manager 가 관리하던 이전 레벨의 카메라 목록을 클리어한다.
	CRenderMgr::GetInst()->ClearCamera();

	// 물리 시뮬레이션 객체를 모두 제거한다.
	CPhysxMgr::GetInst()->ClearScene();


	if (m_CurLevel == _NextLevel || _NextLevel == nullptr)
		return;

	delete m_CurLevel;

	m_CurLevel = _NextLevel;

	if (m_CurLevel == m_PlayStartLevel)
		m_PlayStartLevel = nullptr;

	if (_Play)
		CTaskMgr::GetInst()->AddTask(TASK_TYPE::CHANGE_LEVEL_STATE, (DWORD_PTR)LEVEL_STATE::PLAY, NULL);
}

CGameObject* CLevelMgr::FindObject(LAYER _Layer, wstring _Name)
{
	const auto& vec = m_CurLevel->GetLayer(static_cast<int>(_Layer))->GetObjects();
	for (const auto pObj : vec)
	{
		if (pObj->GetName() == _Name)
			return pObj;
	}
	return nullptr;
}

void CLevelMgr::DeleteObject(LAYER _Layer, wstring _Name)
{
	delete FindObject(_Layer, _Name);
}
