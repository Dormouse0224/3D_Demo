#pragma once

#define REGISTER_SCRIPT(Type) CComponentMgr::GetInst()->RegisterScript(typeid(Type).name()\
, []() -> CScript* { CScript* pScript = new Type; pScript->SetName(to_wstr(typeid(Type).name())); return pScript; });
#define REGISTER_STATE(Type) CFSMMgr::GetInst()->RegisterState(typeid(Type).name()\
, []() -> CFSM_State* { CFSM_State* pState = new Type; pState->SetName(to_wstr(typeid(Type).name())); return pState; });
#define REGISTER_TRIGGER(Func) CFSMMgr::GetInst()->RegisterTrigger(#Func, &Func);

// ========================================================
// 이 코드는 CodeGen 코드 생성기에 의해 작성되었습니다.
// Exeptionlist.txt 는 해당 파일 내 이름들을 예외처리합니다.
// ScriptList.txt 는 코드가 생성된 스크립트와 상태 클래스 목록입니다.
// FuncList.txt 는 코드가 생성된 상태 변경 함수 목록입니다.
// 코드는 항상 갱신됩니다.
// ========================================================

class CScriptInit
{
public:
	static void ScriptInit();
	static void FSMInit();
};

