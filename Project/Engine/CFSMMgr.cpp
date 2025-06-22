#include "pch.h"
#include "CFSMMgr.h"

CFSMMgr::CFSMMgr()
    : m_StateRegistry{}
    , m_TriggerRegistry{}
    , m_FSMInitFunc(nullptr)
{
}

CFSMMgr::~CFSMMgr()
{
}

void CFSMMgr::Init()
{
    if (m_FSMInitFunc)
    {
        m_FSMInitFunc();
    }
}

TriggerFunc CFSMMgr::GetTriggerFunc(string _FuncName)
{
    auto iter = m_TriggerRegistry.find(_FuncName);
    if (iter == m_TriggerRegistry.end())
        return nullptr;
    else
        return iter->second;
}

vector<const char*> CFSMMgr::GetStateRegistryList()
{
    vector<const char*> vec;
    for (const auto& pair : m_StateRegistry)
        vec.push_back(pair.first.data());
    return vec;
}

vector<const char*> CFSMMgr::GetTriggerRegistryList()
{
    vector<const char*> vec;
    for (const auto& pair : m_TriggerRegistry)
        vec.push_back(pair.first.data());
    return vec;
}

void CFSMMgr::RegisterState(const std::string& className, StateCreateFunc func)
{
    auto iter = m_StateRegistry.find(className);
    if (iter != m_StateRegistry.end())
        return;
    m_StateRegistry.insert(make_pair(className, func));
}

void CFSMMgr::RegisterTrigger(const std::string& funcName, TriggerFunc func)
{
    auto iter = m_TriggerRegistry.find(funcName);
    if (iter != m_TriggerRegistry.end())
        return;
    m_TriggerRegistry.insert(make_pair(funcName, func));
}

CFSM_State* CFSMMgr::CreateState(const std::string& className)
{
    auto iter = m_StateRegistry.find(className);
    if (iter != m_StateRegistry.end())
    {
        return iter->second();
    }
    return nullptr;
}
