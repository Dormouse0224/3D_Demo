#pragma once
#include "CComponentUI.h"
class CLightUI :
    public CComponentUI
{
public:
    CLightUI();
    ~CLightUI();

private:


public:
    virtual void Update_Com();
    virtual void Render_Com();
};
