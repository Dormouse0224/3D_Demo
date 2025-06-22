#pragma once
#include "CImguiObject.h"

class CListUI :
    public CImguiObject
{
public:
    CListUI(wstring _Name);
    ~CListUI();
    CLONE_DISABLE(CListUI);

private:
    vector<wstring> m_vecList;
    int m_Idx;
    int m_Selected;

public:
    virtual void Update();
    virtual void Render();

    void Init();

    int GetSelected() { return m_Selected; }

    void SetList(vector<wstring> _vec) { m_vecList = _vec; }

};

