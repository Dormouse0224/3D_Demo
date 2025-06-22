#pragma once
#include "CAssetUI.h"
class CComputeShaderUI :
    public CAssetUI
{
public:
    CComputeShaderUI();
    ~CComputeShaderUI();

private:

public:
    virtual void Update_Ast();
    virtual void Render_Ast();
};

