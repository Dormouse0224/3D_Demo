#pragma once
#include "CRenderComponentUI.h"
class CSpriteRenderUI :
    public CRenderComponentUI
{
public:
    CSpriteRenderUI();
    ~CSpriteRenderUI();

private:


public:
    virtual void Update_Com();
    virtual void Render_Com();
};

