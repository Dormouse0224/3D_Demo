#pragma once
#include "CAsset.h"
class CFont :
    public CAsset
{
public:
    CFont();
    ~CFont();
    CLONE_DISABLE(CFont);

private:
    std::unique_ptr<SpriteBatch> m_SpriteBatch;
    std::unique_ptr<SpriteFont>  m_SpriteFont;

public:

    virtual int Save(const wstring& _FileName, bool _Update = false) { return S_OK; }
    virtual int Load(const wstring& _FilePath);

    void Render(wstring _Text, Vec2 _Pos, Vec4 _Color, float _Rot, float _Scale, float _Depth);
};