#pragma once
#include "CEntity.h"

class CFSM;

class CFSM_State :
    public CEntity
{
public:
    CFSM_State();
    CFSM_State(const CFSM_State& _Other);
    ~CFSM_State();
    virtual CFSM_State* Clone() = 0;

protected:
    CFSM* m_Owner;
    // 상태 상수
    vector<int> m_vecInt;
    vector<float> m_vecFloat;
    vector<Vec2> m_vecVec2;
    vector<Vec4> m_vecVec4;
    vector<DWORD_PTR> m_vecData;


public:
    virtual void Tick() = 0;
    virtual void Begin() = 0;
    virtual void End() = 0;

    CFSM* GetOwner() { return m_Owner; }
    template<typename T>
    bool GetConst(int _Idx, T* _out);
    template<typename T>
    const vector<T>& GetConstVec();

    void SetOwner(CFSM* _Owner) { m_Owner = _Owner; }
    template<typename T>
    void SetConst(int _Idx, T _in);
    template<typename T>
    void SetConstVec(const vector<T>& _ConstVec);


};

template<typename T>
inline void CFSM_State::SetConstVec(const vector<T>& _ConstVec)
{
    if constexpr (std::is_same_v<T, int>)
    {
        m_vecInt = _ConstVec;
    }
    else if constexpr (std::is_same_v<T, float>)
    {
        m_vecFloat = _ConstVec;
    }
    else if constexpr (std::is_same_v<T, Vec2>)
    {
        m_vecVec2 = _ConstVec;
    }
    else if constexpr (std::is_same_v<T, Vec4>)
    {
        m_vecVec4 = _ConstVec;
    }
    else if constexpr (std::is_same_v<T, DWORD_PTR>)
    {
        m_vecData = _ConstVec;
    }
}

template<typename T>
inline const vector<T>& CFSM_State::GetConstVec()
{
    if constexpr (std::is_same_v<T, int>)
    {
        return m_vecInt;
    }
    else if constexpr (std::is_same_v<T, float>)
    {
        return m_vecFloat;
    }
    else if constexpr (std::is_same_v<T, Vec2>)
    {
        return m_vecVec2;
    }
    else if constexpr (std::is_same_v<T, Vec4>)
    {
        return m_vecVec4;
    }
    else if constexpr (std::is_same_v<T, DWORD_PTR>)
    {
        return m_vecData;
    }
}

template<typename T>
inline void CFSM_State::SetConst(int _Idx, T _in)
{
    if constexpr (std::is_same_v<T, int>)
    {
        if (m_vecInt.size() <= _Idx)
            m_vecInt.resize(_Idx + 1);
        m_vecInt[_Idx] = _in;
    }
    else if constexpr (std::is_same_v<T, float>)
    {
        if (m_vecFloat.size() <= _Idx)
            m_vecFloat.resize(_Idx + 1);
        m_vecFloat[_Idx] = _in;
    }
    else if constexpr (std::is_same_v<T, Vec2>)
    {
        if (m_vecVec2.size() <= _Idx)
            m_vecVec2.resize(_Idx + 1);
        m_vecVec2[_Idx] = _in;
    }
    else if constexpr (std::is_same_v<T, Vec4>)
    {
        if (m_vecVec4.size() <= _Idx)
            m_vecVec4.resize(_Idx + 1);
        m_vecVec4[_Idx] = _in;
    }
    else if constexpr (std::is_same_v<T, DWORD_PTR>)
    {
        if (m_vecData.size() <= _Idx)
            m_vecData.resize(_Idx + 1);
        m_vecData[_Idx] = _in;
    }
}

template<typename T>
inline bool CFSM_State::GetConst(int _Idx, T* _out)
{
    if constexpr (std::is_same_v<T, int>)
    {
        if (m_vecInt.size() > _Idx)
        {
            *_out = m_vecInt[_Idx];
            return true;
        }
        else
            return false;
    }
    else if constexpr (std::is_same_v<T, float>)
    {
        if (m_vecFloat.size() > _Idx)
        {
            *_out = m_vecFloat[_Idx];
            return true;
        }
        else
            return false;
    }
    else if constexpr (std::is_same_v<T, Vec2>)
    {
        if (m_vecVec2.size() > _Idx)
        {
            *_out = m_vecVec2[_Idx];
            return true;
        }
        else
            return false;
    }
    else if constexpr (std::is_same_v<T, Vec4>)
    {
        if (m_vecVec4.size() > _Idx)
        {
            *_out = m_vecVec4[_Idx];
            return true;
        }
        else
            return false;
    }
    else if constexpr (std::is_same_v<T, DWORD_PTR>)
    {
        if (m_vecData.size() > _Idx)
        {
            *_out = m_vecData[_Idx];
            return true;
        }
        else
            return false;
    }
}