#pragma once
#include "CComputeShader.h"

#include "CTexture.h"


class CStructuredBuffer;

class CWeightMapCS :
    public CComputeShader
{
private:
    CStructuredBuffer*  m_WeightMap;
    AssetPtr<CTexture>       m_BrushTex;
    CStructuredBuffer*  m_RaycastOut;
    Vec2                m_BrushScale;
    int                 m_WeightIdx;

    int                 m_WeightMapWidth;
    int                 m_WeightMapHeight;

public:
    void SetBrushPos(CStructuredBuffer* _Buffer) { m_RaycastOut = _Buffer; }
    void SetBrushScale(Vec2 _Scale) { m_BrushScale = _Scale; }
    void SetWeightIdx(int _Idx) { m_WeightIdx = _Idx; }
    void SetWeightMapWidthHeight(UINT _Width, UINT _Height) { m_WeightMapWidth = (int)_Width; m_WeightMapHeight = (int)_Height; }
    void SetWeightMap(CStructuredBuffer* _WeightMap) { m_WeightMap = _WeightMap; }
    void SetBrushTex(AssetPtr<CTexture> _BrushTex) { m_BrushTex = _BrushTex; }

public:
    virtual int Binding() override;
    virtual void CalcGroupNum()override;
    virtual void Clear() override;
    virtual void UIRender() override;
    virtual void SetDataInfo() override;

public:
    CWeightMapCS();
    ~CWeightMapCS();
};

