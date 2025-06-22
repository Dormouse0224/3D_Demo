#pragma once
#include "CEntity.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_impl_dx11.h"

class CImguiObject :
    public CEntity
{
public:
    CImguiObject(wstring _Name);
    ~CImguiObject();
    CImguiObject* Clone() = 0;

private:
    CImguiObject*           m_Parent;
    vector<CImguiObject*>   m_vecChild;
    string                  m_Key;
    bool                    m_Active;
    bool                    m_Modal;
    bool                    m_Seperator;
    ImVec2                  m_ChildSize;

public:
    void Update_Progress();
    void Render_Progress();

    virtual void Update() = 0;
    virtual void Render() = 0;

    void AddChildUI(CImguiObject* _ImguiObj) { m_vecChild.push_back(_ImguiObj); }

    CImguiObject* GetParent() { return m_Parent; }
    const string& GetKey() { return m_Key; }
    bool GetActive() { return m_Active; }


    void SetActive(bool _Active) { m_Active = _Active; }
};

