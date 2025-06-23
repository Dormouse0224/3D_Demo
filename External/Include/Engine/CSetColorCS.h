#pragma once
#include "CComputeShader.h"

#include "CTexture.h"

class CSetColorCS :
    public CComputeShader
{
private:
    AssetPtr<CTexture>   m_TargetTex;
    Vec4            m_Color;

public:
    void SetTargetTex(AssetPtr<CTexture> _Tex) { m_TargetTex = _Tex; }
    void SetColor(Vec4 _Color) { m_Color = _Color; }

    virtual int Binding() override;
    virtual void CalcGroupNum() override;
    virtual void Clear() override;
    CLONE_DISABLE(CSetColorCS);
public:

    CSetColorCS();
    ~CSetColorCS();
};

