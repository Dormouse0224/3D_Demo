#pragma once
#include "CEntity.h"

class CAsset :
    public CEntity
{
    template<typename T>
    friend class AssetPtr;

    friend class CAssetMgr;

public:
    CAsset(ASSET_TYPE _Type);
    CAsset(const CAsset& _Other);
    ~CAsset();
    virtual CAsset* Clone() = 0;

private:
    const ASSET_TYPE    m_Type;
    int                 m_RefCount; // 참조 횟수(0 이되면 스스로 삭제)

    wstring             m_Key;
    wstring             m_RelativePath;
    bool                m_EngineAsset;

public:
    ASSET_TYPE GetType() const { return m_Type; }
    wstring GetKey() { return m_Key; }
    wstring GetRelativePath() { return m_RelativePath; }
    bool GetEngineAsset() { return m_EngineAsset; }

    void SetEngineAsset(bool _b) { m_EngineAsset = _b; }

    virtual int Save(const wstring& _FileName, bool _Update = false) = 0;
    virtual int Load(const wstring& _FilePath) = 0;

private:
    void AddRef() { ++m_RefCount; }
    void Release()
    {
        --m_RefCount;
        if (m_RefCount <= 0)
        {
            delete this;
        }
    }

};

