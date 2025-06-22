#pragma once
#include "CRenderComponentUI.h"
#include "CTileRender.h"

class CTileRnederUI :
    public CRenderComponentUI
{
public:
    CTileRnederUI();
    ~CTileRnederUI();

private:
    bool    m_TIlemapEditorOpen;

public:
    virtual void Update_Com();
    virtual void Render_Com();

    void TIlemapEditor(UINT& _Col, UINT& _Row, Vec2& _TileSize, AssetPtr<CTexture2D> _AtlasTex, vector<TileInfo>& _vecTIleInfo);
};

