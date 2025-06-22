#include "pch.h"
#include "CFlipbook.h"

#include "CPathMgr.h"
#include "CAssetMgr.h"

CFlipbook::CFlipbook()
    : CAsset(ASSET_TYPE::FLIPBOOK)
{
}

CFlipbook::~CFlipbook()
{
}

int CFlipbook::GetSceneCount()
{
    if (m_SceneType == SceneType::SPRITE)
    {
        return m_vecSprite.size();
    }
    else if (m_SceneType == SceneType::TEXTURE)
    {
        return m_vecTex.size();
    }
}

int CFlipbook::Save(const wstring& _FileName, bool _Update)
{
    std::filesystem::path path = CPathMgr::GetContentDir() + std::wstring(L"Flipbook\\") + _FileName + std::wstring(L".flip");
    CPathMgr::CreateParentDir(path);
    std::fstream file(path, std::ios::out | std::ios::binary);

    // 플립북 타입 저장
    file.write(reinterpret_cast<char*>(&m_SceneType), sizeof(SceneType));

    if (m_SceneType == SceneType::SPRITE)
    {
        // 스프라이트의 키값(이름) 저장
        int size = m_vecSprite.size();
        file.write(reinterpret_cast<char*>(&size), sizeof(int));
        for (int i = 0; i < size; ++i)
        {
            std::wstring SpriteName = {};
            if (m_vecSprite[i].Get())
                SpriteName = m_vecSprite[i]->GetName();
            int len = SpriteName.size();
            file.write(reinterpret_cast<char*>(&len), sizeof(int));
            file.write(reinterpret_cast<char*>(SpriteName.data()), sizeof(wchar_t) * len);
        }
    }
    else
    {
        // 텍스쳐의 키값(이름) 저장
        int size = m_vecTex.size();
        file.write(reinterpret_cast<char*>(&size), sizeof(int));
        for (int i = 0; i < size; ++i)
        {
            std::wstring TexName = {};
            if (m_vecTex[i].Get())
                TexName = m_vecTex[i]->GetName();
            int len = TexName.size();
            file.write(reinterpret_cast<char*>(&len), sizeof(int));
            file.write(reinterpret_cast<char*>(TexName.data()), sizeof(wchar_t) * len);
        }
    }

    file.close();

    return S_OK;
}

int CFlipbook::Load(const wstring& _FilePath)
{
    std::filesystem::path path = CPathMgr::GetContentDir() + _FilePath;
    std::fstream file(path, std::ios::in | std::ios::binary);

    // 플립북 타입 불러오기
    file.read(reinterpret_cast<char*>(&m_SceneType), sizeof(SceneType));

    if (m_SceneType == SceneType::SPRITE)
    {
        // 스프라이트의 키값(이름) 불러오기
        int size = 0;
        file.read(reinterpret_cast<char*>(&size), sizeof(int));
        m_vecSprite.resize(size);
        for (int i = 0; i < size; ++i)
        {
            std::wstring SpriteName = {};
            int len = 0;
            file.read(reinterpret_cast<char*>(&len), sizeof(int));
            SpriteName.resize(len);
            file.read(reinterpret_cast<char*>(SpriteName.data()), sizeof(wchar_t) * len);

            // 스프라이트 로드
            m_vecSprite[i] = CAssetMgr::GetInst()->Load<CSprite>(SpriteName);
            if (m_vecSprite[i] == nullptr)
            {
                file.close();
                MessageBoxW(nullptr, L"스프라이트 로딩에 실패하였습니다.", L"Flipbook Load Error", MB_OK);
                return E_FAIL;
            }
        }
    }
    else
    {
        // 텍스쳐의 키값(이름) 불러오기
        int size = 0;
        file.read(reinterpret_cast<char*>(&size), sizeof(int));
        m_vecTex.resize(size);
        for (int i = 0; i < size; ++i)
        {
            std::wstring TexName = {};
            int len = 0;
            file.read(reinterpret_cast<char*>(&len), sizeof(int));
            TexName.resize(len);
            file.read(reinterpret_cast<char*>(TexName.data()), sizeof(wchar_t) * len);

            // 텍스쳐 로드
            m_vecTex[i] = CAssetMgr::GetInst()->Load<CTexture2D>(TexName);
            if (m_vecTex[i] == nullptr)
            {
                file.close();
                MessageBoxW(nullptr, L"텍스쳐 로딩에 실패하였습니다.", L"Flipbook Load Error", MB_OK);
                return E_FAIL;
            }
        }
    }

    file.close();

    return S_OK;
}

AssetPtr<CFlipbook> CFlipbook::Create(const wstring& _Name)
{
    AssetPtr<CFlipbook> pNew = new CFlipbook;
    pNew->SetName(_Name);
    pNew->Save(_Name);
    return pNew;
}
