#pragma once

#include "CFSM.h"
#include "CFSM_State.h"

// 하위 라이브러리에서 구현된 FSM State 의 자식 클래스를 생성해주는 함수입니다.
typedef CFSM_State* (*StateCreateFunc)();

// StateCreateFunc 들을 CComponentMgr 의 레지스트리에 등록해주는 함수입니다.
typedef void (*FSMInit)();

class CFSMMgr
	: public Singleton<CFSMMgr>
{
	SINGLE(CFSMMgr);
private:
	std::map<std::string, StateCreateFunc>		m_StateRegistry;
	std::map<std::string, TriggerFunc>			m_TriggerRegistry;
	FSMInit										m_FSMInitFunc;

public:
	void Init();

	TriggerFunc GetTriggerFunc(string _FuncName);
	vector<const char*> GetStateRegistryList();
	vector<const char*> GetTriggerRegistryList();

	// Engine 하위 라이브러리에서 정의된 State 를 등록해주는 함수를 세팅하고, 초기화를 진행합니다.
	void SetFSMInitFunc(FSMInit _ScriptInitFunc) { m_FSMInitFunc = _ScriptInitFunc; Init(); }

	void RegisterState(const std::string& className, StateCreateFunc func);
	void RegisterTrigger(const std::string& funcName, TriggerFunc func);

	CFSM_State* CreateState(const std::string& className);


};

