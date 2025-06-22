#pragma once

#include "CScript.h"

// 하위 라이브러리에서 구현된 스크립트의 자식 클래스를 생성해주는 함수입니다.
typedef CScript* (*ScriptCreateFunc)();

// ScriptCreateFunc 들을 CComponentMgr 의 레지스트리에 등록해주는 함수입니다.
typedef void (*ScriptInit)();

class CComponentMgr
	: public Singleton<CComponentMgr>
{
	SINGLE(CComponentMgr);
private:
	std::map<std::string, ScriptCreateFunc>		m_ScriptRegistry;
	ScriptInit									m_ScriptInitFunc;

public:
	void Init();

	vector<const char*> GetScriptRegistryList();

	// Engine 하위 라이브러리에서 정의된 스크립트를 등록해주는 함수를 세팅하고, 초기화를 진행합니다.
	void SetScriptInitFunc(ScriptInit _ScriptInitFunc) { m_ScriptInitFunc = _ScriptInitFunc; Init(); }

	void RegisterScript(const std::string& className, ScriptCreateFunc func);

	CScript* CreateScript(const std::string& className);
	CComponent* CreateComp(COMPONENT_TYPE _Type);
   

};

