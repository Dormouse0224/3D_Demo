#pragma once
#include "CAsset.h"

#include "CTexture2D.h"

class CSprite :
    public CAsset
{
public:
    CSprite();
    ~CSprite();
    CLONE_DISABLE(CSprite);

private:
    AssetPtr<CTexture2D>   m_Atlas;
    Vec2            m_LeftTop;
    Vec2            m_Slice;


public:
    void SetAtlasTex(AssetPtr<CTexture2D> _Tex) { m_Atlas = _Tex; }
    void SetLeftTop(Vec2 _LeftTopPixel);
    void SetSlice(Vec2 _SlicePixel);

    AssetPtr<CTexture2D> GetAtlasTex() { return m_Atlas; }
    Vec2 GetLeftTop() { return m_LeftTop; }
    Vec2 GetSlice() { return m_Slice; }

public:
    virtual int Save(const wstring& _FileName, bool _Update = false) override;
    virtual int Load(const wstring& _FilePath) override;

    static AssetPtr<CSprite> Create(const wstring& _Name);

};

