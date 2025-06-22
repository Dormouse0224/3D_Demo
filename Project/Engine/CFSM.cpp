#include "pch.h"
#include "CFSM.h"

#include "CFSMMgr.h"
#include "CLevelMgr.h"
#include "CFSM_State.h"

CFSM::CFSM()
	:CComponent(COMPONENT_TYPE::FSM)
	, m_vecCondition{}
	, m_CurrentState(nullptr)
	, m_PrevState(nullptr)
	, m_InitState(nullptr)
	, m_mapStates{}
{
}

CFSM::CFSM(const CFSM& _Other)
	:CComponent(_Other)
	, m_vecCondition{}
	, m_CurrentState(nullptr)
	, m_InitState(nullptr)
	, m_mapStates{}
{
	for (const auto& cond : _Other.m_vecCondition)
		AddCondition(typeid(*cond->m_OriginState).name(), typeid(*cond->m_DestState).name(), cond->m_FuncName);
	
	if (_Other.m_InitState)
		m_InitState = m_mapStates.find(typeid(*_Other.m_InitState).name())->second.first;
}

CFSM::~CFSM()
{
	// 현재 상태가 존재할 경우 상태를 종료함
	if (m_CurrentState)
		m_CurrentState->End();

	for (int i = m_vecCondition.size(); i > 0; --i)
		DeleteCondition(i - 1);
	m_vecCondition.clear();
}

void CFSM::Begin()
{
	if (m_InitState)
	{
		m_CurrentState = m_InitState;
		m_CurrentState->Begin();
	}
}

void CFSM::End()
{
	if (m_CurrentState)
		m_CurrentState->Begin();
}

void CFSM::FinalTick()
{

	// 등록된 조건 검사 실행
	if (CLevelMgr::GetInst()->GetCurrentLevel()->GetState() == LEVEL_STATE::PLAY)
	{
		for (FSM_Condition* tCond : m_vecCondition)
		{
			if (tCond->m_OriginState == m_CurrentState)
			{
				if (tCond->m_TriggerFunc(tCond->m_OriginState, tCond->m_DestState))
				{
					// 출발지 State 의 End 을 호출
					tCond->m_OriginState->End();
					//현재 State 를 갱신
					m_PrevState = tCond->m_OriginState;
					m_CurrentState = tCond->m_DestState;
					// 상수 데이터 동기화
					m_CurrentState->SetConstVec(m_PrevState->GetConstVec<int>());
					m_CurrentState->SetConstVec(m_PrevState->GetConstVec<float>());
					m_CurrentState->SetConstVec(m_PrevState->GetConstVec<Vec2>());
					m_CurrentState->SetConstVec(m_PrevState->GetConstVec<Vec4>());
					m_CurrentState->SetConstVec(m_PrevState->GetConstVec<DWORD_PTR>());
					// 목적지 State 의 Begin 을 호출
					tCond->m_DestState->Begin();
					break;
				}
			}
		}
	}

	if (m_CurrentState)
		m_CurrentState->Tick();
}

int CFSM::Load(fstream& _Stream)
{
	// 조건 목록 불러오기
	int size = 0;
	_Stream.read(reinterpret_cast<char*>(&size), sizeof(int));
	for (int i = 0; i < size; ++i)
	{
		std::string ClassNameOS = "";
		int size = 0;
		_Stream.read(reinterpret_cast<char*>(&size), sizeof(int));
		ClassNameOS.resize(size);
		_Stream.read(reinterpret_cast<char*>(ClassNameOS.data()), sizeof(char) * size);

		std::string ClassNameDS = "";
		size = 0;
		_Stream.read(reinterpret_cast<char*>(&size), sizeof(int));
		ClassNameDS.resize(size);
		_Stream.read(reinterpret_cast<char*>(ClassNameDS.data()), sizeof(char) * size);

		string FuncName = "";
		int len = 0;
		_Stream.read(reinterpret_cast<char*>(&len), sizeof(int));
		FuncName.resize(len);
		_Stream.read(reinterpret_cast<char*>(FuncName.data()), sizeof(char) * len);

		AddCondition(ClassNameOS, ClassNameDS, FuncName);
	}

	// 초기 상태 불러오기
	std::string ClassName = "";
	size = 0;
	_Stream.read(reinterpret_cast<char*>(&size), sizeof(int));
	if (size > 0)
	{
		ClassName.resize(size);
		_Stream.read(reinterpret_cast<char*>(ClassName.data()), sizeof(char) * size);
		SetInitState(ClassName);
	}

	return S_OK;
}

int CFSM::Save(fstream& _Stream)
{
	// 조건 목록 저장
	int size = m_vecCondition.size();
	_Stream.write(reinterpret_cast<char*>(&size), sizeof(int));
	for (int i = 0; i < m_vecCondition.size(); ++i)
	{
		std::string ClassName = typeid(*(m_vecCondition[i]->m_OriginState)).name();
		int size = ClassName.size();
		_Stream.write(reinterpret_cast<char*>(&size), sizeof(int));
		_Stream.write(reinterpret_cast<char*>(ClassName.data()), sizeof(char) * size);

		ClassName = typeid(*(m_vecCondition[i]->m_DestState)).name();
		size = ClassName.size();
		_Stream.write(reinterpret_cast<char*>(&size), sizeof(int));
		_Stream.write(reinterpret_cast<char*>(ClassName.data()), sizeof(char) * size);

		int len = m_vecCondition[i]->m_FuncName.size();
		_Stream.write(reinterpret_cast<char*>(&len), sizeof(int));
		_Stream.write(reinterpret_cast<char*>(m_vecCondition[i]->m_FuncName.data()), sizeof(char) * len);
	}

	// 초기 상태 저장
	size = 0;
	if (m_InitState)
	{
		std::string ClassName = typeid(*m_InitState).name();
		size = ClassName.size();
		_Stream.write(reinterpret_cast<char*>(&size), sizeof(int));
		_Stream.write(reinterpret_cast<char*>(ClassName.data()), sizeof(char) * size);
	}
	else
		_Stream.write(reinterpret_cast<char*>(&size), sizeof(int));


	return S_OK;
}

void CFSM::SetInitState(string _StateName)
{
	auto iter = m_mapStates.find(_StateName);
	if (iter != m_mapStates.end())
	{
		m_InitState = iter->second.first;
	}
}

void CFSM::AddCondition(const string& _Origin, const string& _Dest, const string& _FuncName)
{
	if (_Origin == _Dest)
		return;

	TriggerFunc func = CFSMMgr::GetInst()->GetTriggerFunc(_FuncName);
	CFSM_State* pOS = nullptr;
	CFSM_State* pDS = nullptr;
	auto iterOS = m_mapStates.find(_Origin);
	if (iterOS == m_mapStates.end())
		pOS = CFSMMgr::GetInst()->CreateState(_Origin);
	else
		pOS = iterOS->second.first;
	auto iterDS = m_mapStates.find(_Dest);
	if (iterDS == m_mapStates.end())
		pDS = CFSMMgr::GetInst()->CreateState(_Dest);
	else
		pDS = iterDS->second.first;
	if (func != nullptr && pOS != nullptr && pDS != nullptr)
	{
		FSM_Condition* pCond = new FSM_Condition;
		pCond->m_OriginState = pOS;
		pCond->m_DestState = pDS;
		pCond->m_FuncName = _FuncName;
		pCond->m_TriggerFunc = func;
		m_vecCondition.push_back(pCond);
		
		if (iterOS != m_mapStates.end())
		{
			++(iterOS->second.second);
		}
		else
		{
			pOS->SetOwner(this);
			m_mapStates.insert(make_pair(_Origin, make_pair(pOS, 1)));
		}

		if (iterDS != m_mapStates.end())
		{
			++(iterDS->second.second);
		}
		else
		{
			pDS->SetOwner(this);
			m_mapStates.insert(make_pair(_Dest, make_pair(pDS, 1)));
		}
	}
}

void CFSM::DeleteCondition(int _Idx)
{
	if (_Idx < m_vecCondition.size())
	{
		auto iter = m_mapStates.find(typeid(*(m_vecCondition[_Idx]->m_OriginState)).name());
		--(iter->second.second);
		if (iter->second.second == 0)
		{
			delete iter->second.first;
			m_mapStates.erase(iter);
		}

		iter = m_mapStates.find(typeid(*(m_vecCondition[_Idx]->m_DestState)).name());
		--(iter->second.second);
		if (iter->second.second == 0)
		{
			delete iter->second.first;
			m_mapStates.erase(iter);
		}

		delete m_vecCondition[_Idx];
		m_vecCondition[_Idx] = nullptr;
		m_vecCondition.erase(m_vecCondition.begin() + _Idx);
	}

	m_InitState = nullptr;
}
