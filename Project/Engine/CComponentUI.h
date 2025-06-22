#pragma once
#include "CImguiObject.h"

class CGameObject;

class CComponentUI :
    public CImguiObject
{
public:
    CComponentUI(COMPONENT_TYPE _Type);
    ~CComponentUI();
    CLONE_DISABLE(CComponentUI);

protected:
    const COMPONENT_TYPE    m_Type;
    CGameObject*            m_TargetObj;

public:
    CGameObject* GetTarget() { return m_TargetObj; }

    void SetTargetObj(CGameObject* _Obj) { m_TargetObj = _Obj; }

    virtual void Update() final;
    virtual void Render() final;

public:
    virtual void Update_Com() = 0;
    virtual void Render_Com() = 0;
};

