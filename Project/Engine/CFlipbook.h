#pragma once
#include "CAsset.h"

#include "CSprite.h"

enum class SceneType
{
    SPRITE,
    TEXTURE,

};

class CFlipbook :
    public CAsset
{
public:
    CFlipbook();
    ~CFlipbook();
    CLONE_DISABLE(CFlipbook);

private:
    vector<AssetPtr<CSprite>>       m_vecSprite;
    vector<AssetPtr<CTexture2D>>    m_vecTex;

    SceneType               m_SceneType;

public:
    template<typename T>
    void AddScene(AssetPtr<T> _Scene, int _SceneIdx)
    {
        if constexpr (is_same_v<CSprite, T>)
        {
            if (m_vecSprite.size() < _SceneIdx)
                return;

            m_vecSprite.insert(m_vecSprite.begin() + _SceneIdx, _Scene);
            m_SceneType = SceneType::SPRITE;
        }
        else if constexpr (is_same_v<CTexture2D, T>)
        {
            if (m_vecTex.size() < _SceneIdx)
                return;

            m_vecTex.insert(m_vecTex.begin() + _SceneIdx, _Scene);
            m_SceneType = SceneType::TEXTURE;
        }
    }

    void DeleteScene(int _SceneIdx)
    {
        if (m_SceneType == SceneType::TEXTURE)
        {
            if (m_vecTex.size() <= _SceneIdx)
                return;

            m_vecTex.erase(m_vecTex.begin() + _SceneIdx);
        }
        else
        {
            if (m_vecSprite.size() <= _SceneIdx)
                return;

            m_vecSprite.erase(m_vecSprite.begin() + _SceneIdx);
        }
    }

    int GetSceneCount();

    template<typename T>
    AssetPtr<T> GetScene(int _Idx)
    {
        if constexpr (is_same_v<CSprite, T>)
        {
            if (_Idx >= m_vecSprite.size() || _Idx < 0)
                return nullptr;
            return m_vecSprite[_Idx];
        }
        else if constexpr (is_same_v<CTexture2D, T>)
        {
            if (_Idx >= m_vecTex.size() || _Idx < 0)
                return nullptr;
            return m_vecTex[_Idx];
        }
    }

    SceneType GetSceneType() { return m_SceneType; }


    void SetSceneType(SceneType _Type) { m_SceneType = _Type; }

public:
    virtual int Save(const wstring& _FileName, bool _Update = false) override;
    virtual int Load(const wstring& _FilePath) override;

    static AssetPtr<CFlipbook> Create(const wstring& _Name);

};

