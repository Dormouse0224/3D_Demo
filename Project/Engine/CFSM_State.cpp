#include "pch.h"
#include "CFSM_State.h"

CFSM_State::CFSM_State()
	: CEntity()
	, m_Owner(nullptr)
	, m_vecInt{}
	, m_vecFloat{}
	, m_vecVec2{}
	, m_vecVec4{}
{
}

CFSM_State::CFSM_State(const CFSM_State& _Other)
	: CEntity(_Other)
	, m_Owner(nullptr)
	, m_vecInt{ _Other.m_vecInt }
	, m_vecFloat{ _Other.m_vecFloat }
	, m_vecVec2{ _Other.m_vecVec2 }
	, m_vecVec4{ _Other.m_vecVec4 }
{
}

CFSM_State::~CFSM_State()
{
}

