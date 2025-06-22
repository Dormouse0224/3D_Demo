#pragma once
#include "CAssetUI.h"
class CGraphicShaderUI :
    public CAssetUI
{
public:
    CGraphicShaderUI();
    ~CGraphicShaderUI();

private:

public:
    virtual void Update_Ast();
    virtual void Render_Ast();
};

