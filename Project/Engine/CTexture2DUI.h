#pragma once
#include "CAssetUI.h"
class CTexture2DUI :
    public CAssetUI
{
public:
    CTexture2DUI();
    ~CTexture2DUI();

private:

public:
    virtual void Update_Ast();
    virtual void Render_Ast();
};

