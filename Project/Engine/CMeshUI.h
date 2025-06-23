#pragma once
#include "CAssetUI.h"

class CGameObject;
class CTexture;

class CMeshUI :
    public CAssetUI
{
public:
    CMeshUI();
    ~CMeshUI();

private:
    CGameObject*            m_MeshCam;
    CGameObject*            m_VertexObject;
    AssetPtr<CTexture>    m_VertexRTTex;
    AssetPtr<CTexture>    m_VertexDSTex;
    D3D11_VIEWPORT          m_ViewPort;


public:
    virtual void Update_Ast();
    virtual void Render_Ast();

    void VertexRender();
};

