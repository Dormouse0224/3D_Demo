#pragma once
#include "CAssetUI.h"
class CFlipbookUI :
    public CAssetUI
{
public:
    CFlipbookUI();
    ~CFlipbookUI();

private:

public:
    virtual void Update_Ast();
    virtual void Render_Ast();
};

