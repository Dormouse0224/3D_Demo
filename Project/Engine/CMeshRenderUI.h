#pragma once
#include "CRenderComponentUI.h"
class CMeshRenderUI :
    public CRenderComponentUI
{
public:
    CMeshRenderUI();
    ~CMeshRenderUI();

private:


public:
    virtual void Update_Com();
    virtual void Render_Com();
};

