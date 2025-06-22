#pragma once
#include "CImguiObject.h"

class CTreeUI;

class CContentViewer :
    public CImguiObject
{
public:
    CContentViewer(wstring _Name);
    ~CContentViewer();
    CLONE_DISABLE(CContentViewer);

private:
    CTreeUI* m_AssetTree;

public:
    virtual void Update();
    virtual void Render();

    void ClickEventCallback(DWORD_PTR _Target);

    void Renew();
};

