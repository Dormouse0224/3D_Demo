#pragma once
#include "CRenderComponentUI.h"
class CParticleRenderUI :
    public CRenderComponentUI
{
public:
    CParticleRenderUI();
    ~CParticleRenderUI();

private:


public:
    virtual void Update_Com();
    virtual void Render_Com();
};

