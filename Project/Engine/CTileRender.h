#pragma once
#include "CRenderComponent.h"

#include "CSprite.h"
#include "CTexture2D.h"
#include "CStructuredBuffer.h"

struct TileInfo {
    Vec2 vLeftTop;

    int padding[2];
};

class CTileRender :
    public CRenderComponent
{
    friend class CTileRnederUI;
public:
    CTileRender();
    CTileRender(const CTileRender& _Other);
    ~CTileRender();
    CLONE(CTileRender);

private:
    UINT                    m_Col;          // 타일맵 열 개수
    UINT                    m_Row;          // 타일맵 행 개수
    Vec2                    m_TileSize;     // 타일 1개의 크기
    AssetPtr<CTexture2D>    m_AtlasTex;     // 타일의 아틀라스 텍스쳐
    vector<TileInfo>        m_vecTileInfo;  // 각 타일이 가리키는 스프라이트의 Left Top 좌표

    CStructuredBuffer*      m_GpuBuffer;    // 타일 스프라이트의 정보를 GPU로 전달하는 구조화버퍼


public:
    void SetColRow(UINT _Col, UINT _Row);
    void SetTileSize(Vec2 _TileSize);
    void SetAtlasTex(AssetPtr<CTexture2D> _Tex) { m_AtlasTex = _Tex; }
    void SetTile(UINT _Col, UINT _Row, Vec2 _LT);

public:
    virtual void FinalTick() override;
    virtual void Render() override;

    virtual int Load(fstream& _Stream) override;
    virtual int Save(fstream& _Stream) override;

private:
    void UpdateBuffer();



};

