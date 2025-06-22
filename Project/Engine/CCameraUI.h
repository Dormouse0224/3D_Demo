#pragma once
#include "CComponentUI.h"
class CCameraUI :
    public CComponentUI
{
public:
    CCameraUI();
    ~CCameraUI();

private:


public:
    virtual void Update_Com();
    virtual void Render_Com();
};

