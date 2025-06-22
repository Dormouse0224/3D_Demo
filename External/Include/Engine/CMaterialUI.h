#pragma once
#include "CAssetUI.h"
class CMaterialUI :
    public CAssetUI
{
public:
    CMaterialUI();
    ~CMaterialUI();

private:

public:
    virtual void Update_Ast();
    virtual void Render_Ast();
};

