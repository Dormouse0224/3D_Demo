#pragma once
#include "CComputeShader.h"

#include "CTexture.h"

class CParticleTickCS :
    public CComputeShader
{
public:
    CParticleTickCS();
    ~CParticleTickCS();

private:
    CStructuredBuffer* m_ParticleBuffer;
    CStructuredBuffer* m_SpawnBuffer;
    CStructuredBuffer* m_ModuleBuffer;
    //AssetPtr<CTexture2D>       m_NoiseTex;

public:
    void SetParticleBuffer(CStructuredBuffer* _Buffer) { m_ParticleBuffer = _Buffer; }
    void SetSpawnBuffer(CStructuredBuffer* _Buffer) { m_SpawnBuffer = _Buffer; }
    void SetModuleBuffer(CStructuredBuffer* _Buffer) { m_ModuleBuffer = _Buffer; }
    void SetNoiseTex(AssetPtr<CTexture> _Noise) { m_CSTex[TEX_0] = make_pair(_Noise, 0); }

public:
    virtual void UIRender() override {}
    virtual void SetDataInfo() override;

    virtual int Binding() override;
    virtual void CalcGroupNum() override;
    virtual void Clear() override;

};

