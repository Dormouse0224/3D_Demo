#pragma once

#include "CRenderComponent.h"

class CSkyBox :
    public CRenderComponent
{
public:
    CSkyBox();
    CSkyBox(const CSkyBox& _Other);
    ~CSkyBox();
    CLONE(CSkyBox);

private:
    AssetPtr<CTexture> m_SkyboxTex;

public:
    virtual void FinalTick() override;
    virtual void Render() override;

    virtual int Load(fstream& _Stream) override;
    virtual int Save(fstream& _Stream) override;

public:
    void SetSkyboxTex(AssetPtr<CTexture> _Tex) { m_SkyboxTex = _Tex; }
};

