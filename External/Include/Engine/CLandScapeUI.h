#pragma once
#include "CRenderComponentUI.h"
class CLandScapeUI :
    public CRenderComponentUI
{
public:
    CLandScapeUI();
    ~CLandScapeUI();

private:


public:
    virtual void Update_Com();
    virtual void Render_Com();
};
