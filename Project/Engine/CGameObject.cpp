#include "pch.h"
#include "CGameObject.h"
#include "components.h"
#include "CScript.h"

#include "CLevelMgr.h"
#include "CImguiMgr.h"
#include "CPathMgr.h"
#include "CComponentMgr.h"

#include "CLevel.h"
#include "CLayer.h"
#include "CRenderComponent.h"
#include "CInspector.h"

CGameObject::CGameObject()
	: m_Com{}
	, m_vecScript{}
	, m_RenderCom(nullptr)
	, m_Parent(nullptr)
	, m_vecChild{}
	, m_LayerIdx(-1)
	, m_Dead(false)
{
}

CGameObject::CGameObject(const CGameObject& _Other)
	: CEntity(_Other)
	, m_Com{}
	, m_vecScript{}
	, m_RenderCom(nullptr)
	, m_Parent(nullptr)
	, m_vecChild{}
	, m_LayerIdx(_Other.m_LayerIdx)
	, m_Dead(false)
{
	for (int i = 0; i < (UINT)COMPONENT_TYPE::COMPONENT_END; ++i)
	{
		if (_Other.m_Com[i])
			AddComponent(_Other.m_Com[i]->Clone());
	}

	for (const auto& script : _Other.m_vecScript)
	{
		AddComponent(script->Clone());
	}

	for (const auto& child : _Other.m_vecChild)
	{
		CGameObject* mychild = child->Clone();
		mychild->m_Parent = this;
		m_vecChild.push_back(mychild);
	}
}

CGameObject::~CGameObject()
{
	// Inspector 가 표시중인 경우 표시 해제
	CInspector* Inspector = CImguiMgr::GetInst()->GetInspector();
	if (Inspector)
	{
		if (Inspector->GetTargetObject() == this)
		{
			Inspector->SetTargetObject(nullptr);
		}
	}

	for (UINT i = 0; i < (UINT)COMPONENT_TYPE::COMPONENT_END; ++i)
	{
		if (nullptr != m_Com[i])
		{
			delete m_Com[i];
			m_Com[i] = nullptr;
		}
	}
	Delete_Vec(m_vecScript);
	Delete_Vec(m_vecChild);

}

void CGameObject::AddComponent(CComponent* _Component)
{
	if (!_Component)
		return;

	COMPONENT_TYPE Type = _Component->GetType();

	if (Type == COMPONENT_TYPE::SCRIPT)
	{
		m_vecScript.push_back((CScript*)_Component);
		m_mapScript.insert(make_pair(typeid(*(CScript*)_Component).name(), (CScript*)_Component));
	}
	else
	{
		// 이미 가지고 있는 컴포넌트인 경우 입력 무시
		if (m_Com[(UINT)Type])
			return;

		if (dynamic_cast<CRenderComponent*>(_Component))
		{
			// 하나의 GameObject 는 한종류의 RenderComonent 만 가질 수 있다.
			// 이미 RenderComponent 를 보유중인 경우
			if (nullptr != m_RenderCom)
			{
				MessageBoxW(nullptr, L"이미 Render Component 를 보유중입니다.", L"Add Component Error", MB_OK);
				return;
			}

			m_RenderCom = (CRenderComponent*)_Component;
		}

		m_Com[(UINT)Type] = _Component;
	}

	_Component->m_Owner = this;
}

void CGameObject::Begin()
{
	for (UINT i = 0; i < (UINT)COMPONENT_TYPE::COMPONENT_END; ++i)
	{
		if (nullptr != m_Com[i])
			m_Com[i]->Begin();
	}

	for (size_t i = 0; i < m_vecScript.size(); ++i)
	{
		m_vecScript[i]->Begin();
	}

	for (size_t i = 0; i < m_vecChild.size(); ++i)
	{
		m_vecChild[i]->Begin();
	}
}

void CGameObject::End()
{
	for (UINT i = 0; i < (UINT)COMPONENT_TYPE::COMPONENT_END; ++i)
	{
		if (nullptr != m_Com[i])
			m_Com[i]->End();
	}

	for (size_t i = 0; i < m_vecScript.size(); ++i)
	{
		m_vecScript[i]->End();
	}

	for (size_t i = 0; i < m_vecChild.size(); ++i)
	{
		m_vecChild[i]->End();
	}
}

void CGameObject::Tick()
{
	for (UINT i = 0; i < (UINT)COMPONENT_TYPE::COMPONENT_END; ++i)
	{
		if (nullptr != m_Com[i])
			m_Com[i]->Tick();
	}

	for (size_t i = 0; i < m_vecScript.size(); ++i)
	{
		m_vecScript[i]->Tick();
	}


	for (size_t i = 0; i < m_vecChild.size(); ++i)
	{
		m_vecChild[i]->Tick();
	}
}

void CGameObject::RegisterToLevel()
{
	// Layer 에 GameObject 등록하기
	CLevel* pCurLevel = CLevelMgr::GetInst()->GetCurrentLevel();
	CLayer* pMyLayer = pCurLevel->GetLayer(m_LayerIdx);
	pMyLayer->RegisterGameObject(this);

	for (CGameObject* child : m_vecChild)
	{
		child->RegisterToLevel();
	}
}

void CGameObject::FinalTick()
{
	for (UINT i = 0; i < (UINT)COMPONENT_TYPE::COMPONENT_END; ++i)
	{
		if (nullptr != m_Com[i])
			m_Com[i]->FinalTick();
	}

	for (vector<CGameObject*>::iterator iter = m_vecChild.begin(); iter != m_vecChild.end(); )
	{
		(*iter)->FinalTick();

		if ((*iter)->IsDead())
			iter = m_vecChild.erase(iter);
		else
			++iter;
	}
}

void CGameObject::Render()
{
	if (IsRenderable(this))
		m_RenderCom->Render();
	
	//if (auto pCom = m_Com[static_cast<int>(COMPONENT_TYPE::UICOM)]; pCom)
	//	static_cast<CUICom*>(pCom)->Render();
	
}

int CGameObject::Save(fstream& _Stream, bool _Update)
{
	// m_RenderCom 는 컴포넌트 로드 시 해당 컴포넌트가 렌더 컴포넌트면 오브젝트에 추가하면서 자동으로 등록됨.

	if (!_Stream.is_open())
		return E_FAIL;

	// 자신의 이름 저장
	SaveName(_Stream);

	// 보유한 컴포넌트 정보 저장
	int ComCount = 0;
	for (int i = 0; i < (UINT)COMPONENT_TYPE::COMPONENT_END; ++i)
		if (m_Com[i])
			++ComCount;
	_Stream.write(reinterpret_cast<char*>(&ComCount), sizeof(int));
	for (int i = 0; i < (UINT)COMPONENT_TYPE::COMPONENT_END; ++i)
	{
		if (m_Com[i])
		{
			int comIdx = i;
			if (_Update)
			{
				auto iter = COMPONENT_TYPE_SAVE_MAP.find(COMPONENT_TYPE_WSTR[i]);
				if (iter == COMPONENT_TYPE_SAVE_MAP.end())
					return E_FAIL;

				comIdx = (int)iter->second;
			}
			_Stream.write(reinterpret_cast<char*>(&comIdx), sizeof(int));
			if (FAILED(m_Com[i]->Save(_Stream)))
				return E_FAIL;
		}
	}

	// 보유한 스크립트 정보 저장
	int count = m_vecScript.size();
	_Stream.write(reinterpret_cast<char*>(&count), sizeof(int));
	for (int i = 0; i < count; ++i)
	{
		std::string ClassName = typeid(*m_vecScript[i]).name();
		int size = ClassName.size();
		_Stream.write(reinterpret_cast<char*>(&size), sizeof(int));
		_Stream.write(reinterpret_cast<char*>(ClassName.data()), sizeof(char) * size);

		if (FAILED(m_vecScript[i]->Save(_Stream)))
			return E_FAIL;
	}

	// 소속 레이어 인덱스 저장
	_Stream.write(reinterpret_cast<char*>(&m_LayerIdx), sizeof(int));
	
	// 자식 오브젝트 정보 저장 (재귀)
	count = m_vecChild.size();
	_Stream.write(reinterpret_cast<char*>(&count), sizeof(int));
	for (int i = 0; i < count; ++i)
		m_vecChild[i]->Save(_Stream, _Update);

	return S_OK;
}

int CGameObject::Load(fstream& _Stream)
{
	if (!_Stream.is_open())
		return E_FAIL;

	// 자신의 이름 불러오기
	LoadName(_Stream);

	// 보유한 컴포넌트 정보 불러오기
	int ComCount = 0;
	std::streampos pos = _Stream.tellg();
	_Stream.read(reinterpret_cast<char*>(&ComCount), sizeof(int));
	for (int i = 0; i < ComCount; ++i)
	{
		int ComType = 0;
		_Stream.read(reinterpret_cast<char*>(&ComType), sizeof(int));
		AddComponent(CComponentMgr::GetInst()->CreateComp((COMPONENT_TYPE)ComType));
		if (FAILED(m_Com[ComType]->Load(_Stream)))
			return E_FAIL;
	}

	// 보유한 스크립트 정보 불러오기
	int count = 0;
	_Stream.read(reinterpret_cast<char*>(&count), sizeof(int));
	for (int i = 0; i < count; ++i)
	{
		std::string ClassName = {};
		int size = 0;
		_Stream.read(reinterpret_cast<char*>(&size), sizeof(int));
		ClassName.resize(size);
		_Stream.read(reinterpret_cast<char*>(ClassName.data()), sizeof(char) * size);
		AddComponent(CComponentMgr::GetInst()->CreateScript(ClassName));

		if (m_vecScript[i])
		{
			if (FAILED(m_vecScript[i]->Load(_Stream)))
				return E_FAIL;
		}
		else
			return E_FAIL;
	}

	// 소속 레이어 인덱스 불러오기
	_Stream.read(reinterpret_cast<char*>(&m_LayerIdx), sizeof(int));

	// 자식 오브젝트 정보 불러오기 (재귀)
	count = 0;
	_Stream.read(reinterpret_cast<char*>(&count), sizeof(int));
	m_vecChild.resize(count);
	for (int i = 0; i < count; ++i)
	{
		m_vecChild[i] = new CGameObject;
		m_vecChild[i]->m_Parent = this;
		m_vecChild[i]->Load(_Stream);
	}

	return S_OK;
}


void CGameObject::AddChild(CGameObject* _Child)
{
	if (_Child->m_Parent == this)
		return;
	_Child->m_Parent = this;
	if (_Child->m_LayerIdx < 0)
		_Child->m_LayerIdx = m_LayerIdx;
	m_vecChild.push_back(_Child);
}

void CGameObject::DeleteChild(CGameObject* _Child)
{
	for (auto iter = m_vecChild.begin(); iter != m_vecChild.end(); ++iter)
	{
		if (*iter == _Child)
		{
			m_vecChild.erase(iter);
			_Child->m_Parent = nullptr;
			break;
		}
	}
}

CGameObject* CGameObject::FindChild(wstring _Name)
{
	for (int i = 0; i < m_vecChild.size(); ++i)
	{
		if (m_vecChild[i]->GetName() == _Name)
			return m_vecChild[i];
	}
	return nullptr;
}


#include "CTaskMgr.h"
void CGameObject::Destroy()
{
	CTaskMgr::GetInst()->AddTask(TASK_TYPE::DELETE_OBJECT, (DWORD_PTR)this, (DWORD_PTR)nullptr);
}

void CGameObject::ChangeLayer(LAYER _Dest, bool _CHildMove)
{
	if (m_Parent == nullptr)
	{
		CLayer* pPrevLayer = CLevelMgr::GetInst()->GetCurrentLevel()->GetLayer(m_LayerIdx);
		CLayer* pNewLayer = CLevelMgr::GetInst()->GetCurrentLevel()->GetLayer(static_cast<int>(_Dest));
		pPrevLayer->UnRegisterParentObject(this);
		pNewLayer->AddGameObject(this, _CHildMove);
	}
	else
	{
		m_LayerIdx = static_cast<int>(_Dest);
		if (_CHildMove)
		{
			for (const auto& pChild : m_vecChild)
				pChild->ChangeLayer(_Dest, _CHildMove);
		}
	}
}

bool CGameObject::IsAncestorOf(CGameObject* _Obj)
{
	for (CGameObject* pChild : m_vecChild)
	{
		if (pChild == _Obj)
			return true;
		if (pChild->IsAncestorOf(_Obj))
			return true;
	}
	return false;
}

void CGameObject::ConvertToRoot()
{
	if (m_Parent == nullptr)
		return;

	m_Parent->DeleteChild(this);
	m_Parent = nullptr;
	CLevelMgr::GetInst()->GetCurrentLevel()->GetLayer(m_LayerIdx)->AddGameObject(this, false);
	CLevelMgr::GetInst()->SetLevelModified();
}

CScript* CGameObject::FindScript(string _className)
{
	if (auto iter = m_mapScript.find(_className); iter != m_mapScript.end())
	{ 
		return iter->second;
	}

	return nullptr;
}
