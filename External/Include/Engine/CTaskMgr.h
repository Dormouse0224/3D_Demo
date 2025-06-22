#pragma once


enum class TASK_TYPE
{
	CREATE_OBJECT,		// Param0 : CGameObject* NewObject / Param1 : int LayerIdx
	DELETE_OBJECT,		// Param0 : CGameObject* DeleteObject / Param1 : NULL
	CHANGE_LEVEL,		// Param0 : CLevel* DestLevel / Param1 : bool Play
	CHANGE_LEVEL_STATE,	// Param0 :	LEVEL_STATE State / Param1 : NULL
};

struct tTask
{
	TASK_TYPE Type;
	DWORD_PTR Param0;
	DWORD_PTR Param1;
};

class CGameObject;

class CTaskMgr
	: public Singleton<CTaskMgr>
{
	SINGLE(CTaskMgr);
private:
	vector<tTask>			m_vecTask;
	vector<CGameObject*>	m_vecGC;

public:
	void Tick();
	// Task를 추가합니다. Param 입력은 TASK_TYPE 설명을 참고해 주세요.
	void AddTask(TASK_TYPE Type, DWORD_PTR Param0, DWORD_PTR Param1);
	
};

