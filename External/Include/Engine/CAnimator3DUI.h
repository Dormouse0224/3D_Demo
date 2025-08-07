#pragma once
#include "CComponentUI.h"
class CAnimator3DUI :
    public CComponentUI
{
public:
    CAnimator3DUI();
    ~CAnimator3DUI();

private:


public:
    virtual void Update_Com() override;
    virtual void Render_Com() override;
};
