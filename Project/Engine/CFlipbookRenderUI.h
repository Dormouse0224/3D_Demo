#pragma once
#include "CRenderComponentUI.h"
class CFlipbookRenderUI :
    public CRenderComponentUI
{
public:
    CFlipbookRenderUI();
    ~CFlipbookRenderUI();

private:


public:
    virtual void Update_Com();
    virtual void Render_Com();
};

