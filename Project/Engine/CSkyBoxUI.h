#pragma once
#include "CRenderComponentUI.h"
class CSkyBoxUI :
    public CRenderComponentUI
{
public:
    CSkyBoxUI();
    ~CSkyBoxUI();

private:


public:
    virtual void Update_Com();
    virtual void Render_Com();
};

