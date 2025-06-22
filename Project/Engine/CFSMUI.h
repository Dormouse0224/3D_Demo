#pragma once
#include "CComponentUI.h"
class CFSMUI :
    public CComponentUI
{
public:
    CFSMUI();
    ~CFSMUI();

private:


public:
    virtual void Update_Com();
    virtual void Render_Com();
};

