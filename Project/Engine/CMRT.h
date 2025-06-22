#pragma once
#include "CEntity.h"

class CTexture2D;

class CMRT :
    public CEntity
{
public:
    CMRT();
    CMRT(const CMRT& _Other);
    ~CMRT();
    CLONE(CMRT);

private:
    D3D11_VIEWPORT          m_Viewport;
    AssetPtr<CTexture2D>    m_RTTex[8];
    AssetPtr<CTexture2D>    m_DSTex;
    int                     m_TexCount;
    Vec4                    m_ClearColor[8];
    float                   m_DepthClear;
    UINT8                   m_StencilClear;

public:
    void SetViewport(D3D11_VIEWPORT _VP) { m_Viewport = _VP; }
    void SetRenderTarget();
    AssetPtr<CTexture2D> GetRenderTarget(int _Idx);

    int CreateRenderTarget(D3D11_TEXTURE2D_DESC _Desc, wstring _Key, Vec4 _ClearColor = Vec4(0.f));
    int CreateRenderTarget(ComPtr<ID3D11Texture2D> _Tex, wstring _Key, Vec4 _ClearColor = Vec4(0.f));
    int CreateDepthStencil(D3D11_TEXTURE2D_DESC _Desc, wstring _Key, float _DepthClear = 1.f, UINT8 _StencilClear = 0);

    void ClearTargets();

};

