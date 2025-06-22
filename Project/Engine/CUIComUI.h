#pragma once
#include "CRenderComponentUI.h"
class CUIComUI :
    public CRenderComponentUI
{
public:
    CUIComUI();
    ~CUIComUI();

private:


public:
    virtual void Update_Com();
    virtual void Render_Com();
};
