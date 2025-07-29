#pragma once
#include "CComputeShader.h"

#include "CTexture.h"

class CStructuredBuffer;

class CHeightMapCS :
    public CComputeShader
{
private:
    AssetPtr<CTexture>       m_HeightMap;
    AssetPtr<CTexture>       m_BrushTex;
    Vec2                m_BrushScale;
    CStructuredBuffer*  m_RaycastOut;

public:
    void SetHeightMap(AssetPtr<CTexture> _Texture) { m_HeightMap = _Texture; }
    void SetBrushPos(CStructuredBuffer* _Buffer) { m_RaycastOut = _Buffer; }
    void SetBrushTex(AssetPtr<CTexture> _Tex) { m_BrushTex = _Tex; }
    void SetBrushScale(Vec2 _Scale) { m_BrushScale = _Scale; }

public:
    virtual int Binding() override;
    virtual void CalcGroupNum() override;
    virtual void Clear() override;
    virtual void UIRender() override;
    virtual void SetDataInfo() override;

public:
    CHeightMapCS();
    ~CHeightMapCS();
};

