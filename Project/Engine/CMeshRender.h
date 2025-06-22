#pragma once
#include "CRenderComponent.h"

#include "CMesh.h"
#include "CGraphicShader.h"

class CMeshRender :
    public CRenderComponent
{
public:
    CMeshRender();
    ~CMeshRender();
    CLONE(CMeshRender);

public:
    virtual void FinalTick() override;
    virtual void Render() override;

    virtual int Load(fstream& _Stream) override;
    virtual int Save(fstream& _Stream) override;

};

