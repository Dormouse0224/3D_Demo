#pragma once
#include "CComponent.h"

class CFSM_State;

// State 전환의 조건을 검사하는 함수. 각 State 의 상수값을 통해 내부적으로 조건을 처리하여 State 전환 조건 충족 시 true 를 반환하도록 한다.
typedef bool (*TriggerFunc)(CFSM_State* _Origin, CFSM_State* _Dest);

struct FSM_Condition
{
    FSM_Condition()
        : m_OriginState(nullptr)
        , m_DestState(nullptr)
        , m_FuncName{}
        , m_TriggerFunc(nullptr)
    {
    }

    CFSM_State* m_OriginState;
    CFSM_State* m_DestState;
    string      m_FuncName;
    TriggerFunc m_TriggerFunc;
};

class CFSM :
    public CComponent
{
    friend class CFSMUI;
public:
    CFSM();
    CFSM(const CFSM& _Other);
    ~CFSM();
    CLONE(CFSM);

private:
    vector<FSM_Condition*>              m_vecCondition;
    CFSM_State*                         m_CurrentState;
    CFSM_State*                         m_PrevState;
    CFSM_State*                         m_InitState;
    map<string, pair<CFSM_State*, int>> m_mapStates;    // FSM 컴포넌트가 사용하는 State 관리용 컨테이너. Condition 에서의 State 참조 수를 기록한다.

public:
    virtual void Begin();
    virtual void End();
    virtual void FinalTick() override;
    virtual int Load(fstream& _Stream) override;
    virtual int Save(fstream& _Stream) override;

    CFSM_State* GetCurrentState() { return m_CurrentState; }
    CFSM_State* GetPrevState() { return m_PrevState; }

    void SetInitState(string _StateName);

    void AddCondition(const string& _Origin, const string& _Dest, const string& _FuncName);
    void DeleteCondition(int _Idx);

};
