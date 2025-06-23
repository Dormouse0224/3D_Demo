#pragma once

#include "CRenderComponent.h"

class CSkyBox :
    public CRenderComponent
{
public:
    CSkyBox();
    ~CSkyBox();
    CLONE(CSkyBox);

public:
    virtual void FinalTick() override;
    virtual void Render() override;

    virtual int Load(fstream& _Stream) override;
    virtual int Save(fstream& _Stream) override;
};

