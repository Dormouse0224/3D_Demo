#pragma once
#include "CAssetUI.h"

class CGameObject;
class CTexture2D;

class CMeshUI :
    public CAssetUI
{
public:
    CMeshUI();
    ~CMeshUI();

private:
    CGameObject*            m_MeshCam;
    CGameObject*            m_VertexObject;
    AssetPtr<CTexture2D>    m_VertexRTTex;
    AssetPtr<CTexture2D>    m_VertexDSTex;
    D3D11_VIEWPORT          m_ViewPort;


public:
    virtual void Update_Ast();
    virtual void Render_Ast();

    void VertexRender();
};

