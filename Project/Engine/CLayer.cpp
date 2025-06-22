#include "pch.h"
#include "CLayer.h"

#include "CTaskMgr.h"
#include "CLevelMgr.h"

#include "CGameObject.h"

CLayer::CLayer()
	: m_vecParentObjects{}
	, m_vecObjects{}
	, m_LayerIdx(-1)
{
}

CLayer::CLayer(const CLayer& _Other)
	: CEntity(_Other)
	, m_vecParentObjects{}
	, m_vecObjects{}
	, m_LayerIdx(_Other.m_LayerIdx)
{
	for (const auto& Object : _Other.m_vecParentObjects)
	{
		AddGameObject(Object->Clone(), false);
	}
}

CLayer::~CLayer()
{
	for (int i = 0; i < m_vecParentObjects.size(); ++i)
		CTaskMgr::GetInst()->AddTask(TASK_TYPE::DELETE_OBJECT, reinterpret_cast<DWORD_PTR>(m_vecParentObjects[i]), NULL);
	//Delete_Vec(m_vecParentObjects);
}

void CLayer::Begin()
{
	for (size_t i = 0; i < m_vecParentObjects.size(); ++i)
	{
		m_vecParentObjects[i]->Begin();
	}
}

void CLayer::End()
{
	for (size_t i = 0; i < m_vecParentObjects.size(); ++i)
	{
		m_vecParentObjects[i]->End();
	}
}

void CLayer::Tick()
{
	for (size_t i = 0; i < m_vecParentObjects.size(); ++i)
	{
		m_vecParentObjects[i]->Tick();
	}
}

void CLayer::Register()
{
	for (CGameObject* parent : m_vecParentObjects)
	{
		parent->RegisterToLevel();
	}
}

void CLayer::FinalTick()
{
	vector<CGameObject*>::iterator iter = m_vecParentObjects.begin();

	for (; iter != m_vecParentObjects.end(); )
	{
		(*iter)->FinalTick();

		if ((*iter)->IsDead())
		{
			iter = m_vecParentObjects.erase(iter);
		}
		else
		{
			++iter;
		}
	}
}

void CLayer::AddGameObject(CGameObject* _Parent, bool _bChildMove)
{
	// 전달받은 오브젝트를 레이어의 부모 오브젝트 컨테이너에 저장하고, 오브젝트의 레이어 인덱스 정보를 현재 레이어 인덱스로 수정한다.
	m_vecParentObjects.push_back(_Parent);
	_Parent->m_LayerIdx = m_LayerIdx;

	// 자식을 보유했을 경우, 자식도 해당 레이어 소속으로 변경한다.
	// 오브젝트와 오브젝트의 자식에 대한 작업 처리 큐를 생성한다.
	static list<CGameObject*> queue;
	queue.clear();
	queue.push_back(_Parent);

	while (!queue.empty())
	{
		// 큐에서 가장 앞의 오브젝트를 추출
		CGameObject* pObject = queue.front();
		queue.pop_front();

		// 추출한 오브젝트의 자식이 존재하는 경우, 큐의 뒤에 자식을 추가
		const vector<CGameObject*>& vecChild = pObject->GetChild();
		for (size_t i = 0; i < vecChild.size(); ++i)
		{
			queue.push_back(vecChild[i]);
		}

		// 최상위 부모 오브젝트 
		// or 자식오브젝트여도, 자식까지 같이 이동하기로 한 경우 
		// or 최상위부모오브젝트x, 자식이동x 더라도 소속이 없는 오브젝트인 경우
		if (nullptr == pObject->GetParent() || _bChildMove || pObject->m_LayerIdx == -1)
		{
			pObject->m_LayerIdx = m_LayerIdx;
		}
	}

	CLevelMgr::GetInst()->SetLevelModified();
}

void CLayer::UnRegisterParentObject(CGameObject* _Parent)
{
	for (auto iter = m_vecParentObjects.begin(); iter != m_vecParentObjects.end(); ++iter)
	{
		if (*iter == _Parent)
		{
			m_vecParentObjects.erase(iter);
			CLevelMgr::GetInst()->SetLevelModified();
			break;
		}
	}
}

int CLayer::Save(fstream& _File, bool _Update)
{
	if (!_File.is_open())
		return E_FAIL;

	_File.write(reinterpret_cast<char*>(&m_LayerIdx), sizeof(int));

	int count = m_vecParentObjects.size();
	_File.write(reinterpret_cast<char*>(&count), sizeof(int));
	for (int i = 0; i < count; ++i)
	{
		m_vecParentObjects[i]->Save(_File, _Update);
	}

	return S_OK;
}

int CLayer::Load(fstream& _File)
{
	if (!_File.is_open())
		return E_FAIL;

	_File.read(reinterpret_cast<char*>(&m_LayerIdx), sizeof(int));

	int count = 0;
	_File.read(reinterpret_cast<char*>(&count), sizeof(int));
	m_vecParentObjects.resize(count);
	for (int i = 0; i < count; ++i)
	{
		m_vecParentObjects[i] = new CGameObject;
		m_vecParentObjects[i]->Load(_File);
	}

	return S_OK;
}
