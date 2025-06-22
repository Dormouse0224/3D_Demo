#include "pch.h"
#include "CAssetMgr.h"

#include "CPathMgr.h"
#include "CLevelMgr.h"

#include "CLevel.h"

CAssetMgr::CAssetMgr()
	: m_Renew(false)
	, m_AssetModified(false)
{

}

CAssetMgr::~CAssetMgr()
{

}

void CAssetMgr::Tick()
{
	if (m_Renew)
		m_Renew = false;
	if (m_AssetModified)
	{
		m_AssetModified = false;
		m_Renew = true;
	}
	ContentObserve();
}

void CAssetMgr::AddAsset(const wstring& _Key, AssetPtr<CAsset> _Asset)
{
	ASSET_TYPE Type = _Asset->GetType();

	map<wstring, AssetPtr<CAsset>>::iterator iter = m_mapAsset[(UINT)Type].find(_Key);

	if (iter != m_mapAsset[(UINT)Type].end())
	{
		//MessageBoxW(nullptr, L"해당 이름의 에셋은 이미 등록되어 있습니다.", L"Add Asset Error", MB_OK);
		return;
	}

	_Asset->m_Key = _Key;
	_Asset->SetName(_Key);
	m_mapAsset[(UINT)Type].insert(make_pair(_Key, _Asset));
	m_AssetModified = true;
}

AssetPtr<CTexture2D> CAssetMgr::CreateTexture(const wstring& _Key, UINT _Width, UINT _Height
	, DXGI_FORMAT _Format, UINT _BindFlag, D3D11_USAGE _Usage)
{
	AssetPtr<CTexture2D> pTex = FindAsset<CTexture2D>(_Key);
	assert(nullptr == pTex);	// 키값이 중복된 경우 중단합니다.

	pTex = new CTexture2D;
	if (FAILED(pTex->Create(_Width, _Height, _Format, _BindFlag, _Usage)))
	{
		MessageBox(nullptr, _Key.c_str(), L"텍스쳐 생성 실패", MB_OK);
		return nullptr;
	}

	AddAsset(_Key, pTex.Get());

	return pTex;
}

AssetPtr<CTexture2D> CAssetMgr::CreateTexture(const wstring& _Key, ComPtr<ID3D11Texture2D> _Tex2D)
{
	AssetPtr<CTexture2D> pTex = FindAsset<CTexture2D>(_Key);
	if (nullptr != pTex)
		return pTex;

	pTex = new CTexture2D;
	if (FAILED(pTex->Create(_Tex2D)))
	{
		MessageBox(nullptr, _Key.c_str(), L"텍스쳐 생성 실패", MB_OK);
		return nullptr;
	}

	AddAsset(_Key, pTex.Get());

	return pTex;
}


ASSET_TYPE CAssetMgr::GetAssetType(const wstring& _Path)
{
	path EXT = path(_Path).extension();

	if (EXT == L".mesh")
	{
		return ASSET_TYPE::MESH;
	}
	else if (EXT == L".dds" || EXT == L".DDS"
		|| EXT == L".tga" || EXT == L".TGA"
		|| EXT == L".png" || EXT == L".PNG"
		|| EXT == L".jpg" || EXT == L".JPG"
		|| EXT == L".jpeg" || EXT == L".JPEG"
		|| EXT == L".bmp" || EXT == L".BMP")
	{
		return ASSET_TYPE::TEXTURE2D;
	}
	else if (EXT == L".mtrl")
	{
		return ASSET_TYPE::MATERIAL;
	}
	else if (EXT == L".prefab")
	{
		return ASSET_TYPE::PREFAB;
	}
	else if (EXT == L".wav" || EXT == L".WAV"
		|| EXT == L".mp3" || EXT == L".MP3"
		|| EXT == L".aac" || EXT == L".AAC"
		|| EXT == L".ogg" || EXT == L".OGG"
		|| EXT == L".flac" || EXT == L".FLAC"
		|| EXT == L".wma" || EXT == L".WMA")
	{
		return ASSET_TYPE::SOUND;
	}
	else if (EXT == L".shader")
	{
		return ASSET_TYPE::GRAPHIC_SHADER;
	}
	else if (EXT == L".sprite")
	{
		return ASSET_TYPE::SPRITE;
	}
	else if (EXT == L".flip")
	{
		return ASSET_TYPE::FLIPBOOK;
	}
	else if (EXT == L".spritefont")
	{
		return ASSET_TYPE::FONT;
	}

	return ASSET_TYPE::ASSET_END;
}

AssetPtr<CTexture2D> CAssetMgr::GetNormTex(const wstring& _DefaultTexName)
{
	int pos = _DefaultTexName.rfind(L'.');
	if (pos == wstring::npos)
		return nullptr;
	wstring NormTexName = _DefaultTexName.substr(0, pos) + L"_N" + _DefaultTexName.substr(pos);

	map<wstring, AssetPtr<CAsset>>::iterator iter = m_mapAsset[(UINT)ASSET_TYPE::TEXTURE2D].find(NormTexName);

	if (iter == m_mapAsset[(UINT)ASSET_TYPE::TEXTURE2D].end())
		return nullptr;

	return (CTexture2D*)(iter->second.Get());
}


void CAssetMgr::ContentLoad()
{
	m_Loading = true;
	for (const auto& file : std::filesystem::recursive_directory_iterator(CPathMgr::GetContentDir()))
	{
		if (file.is_regular_file())
		{
			std::wstring rp = file.path().generic_wstring().substr(wstring(CPathMgr::GetContentDir()).size());
			for (int i = 0; i < rp.size(); ++i)
			{
				if (rp[i] == L'/')
					rp[i] = L'\\';
			}
			switch (CAssetMgr::GetInst()->GetAssetType(file.path()))
			{
			case ASSET_TYPE::MESH:
				CAssetMgr::GetInst()->Load<CMesh>(rp);
				break;
			case ASSET_TYPE::MESHDATA:

				break;
			case ASSET_TYPE::TEXTURE2D:
				CAssetMgr::GetInst()->Load<CTexture2D>(rp);
				break;
			case ASSET_TYPE::MATERIAL:
				CAssetMgr::GetInst()->Load<CMaterial>(rp);
				break;
			case ASSET_TYPE::PREFAB:
				CAssetMgr::GetInst()->Load<CPrefab>(rp);
				break;
			case ASSET_TYPE::SOUND:
				CAssetMgr::GetInst()->Load<CSound>(rp);
				break;
			case ASSET_TYPE::GRAPHIC_SHADER:
				CAssetMgr::GetInst()->Load<CGraphicShader>(rp);
				break;
			case ASSET_TYPE::COMPUTE_SHADER:
				break;
			case ASSET_TYPE::SPRITE:
				CAssetMgr::GetInst()->Load<CSprite>(rp);
				break;
			case ASSET_TYPE::FLIPBOOK:
				CAssetMgr::GetInst()->Load<CFlipbook>(rp);
				break;
			case ASSET_TYPE::FSM_STATE:

				break;
			case ASSET_TYPE::FONT:
				CAssetMgr::GetInst()->Load<CFont>(rp);
				break;
			case ASSET_TYPE::ASSET_END:
				break;
			}

		}
	}
	m_AssetModified = true;
	m_Loading = false;
}

void CAssetMgr::ContentObserve()
{
	// 즉시 확인
	DWORD dwStatus = WaitForSingleObject(m_DirNotifyHandle, 0);

	// 지정된 이벤트가 발생했다.
	if (dwStatus == WAIT_OBJECT_0)
	{
		ContentReload();

		FindNextChangeNotification(m_DirNotifyHandle);
	}
}

void CAssetMgr::ContentReload()
{
	// map 에 저장된 에셋을 엔진 에셋 제외 모두 제거하고 다시 로드
	for (auto& map : m_mapAsset)
	{
		for (auto iter = map.begin(); iter != map.end();)
		{
			if (iter->second->GetEngineAsset() == true)
			{
				++iter;
				continue;
			}

			iter = map.erase(iter);
		}
	}
	ContentLoad();
}

void CAssetMgr::UpdateAsset()
{
	std::wstring ContentDir = CPathMgr::GetContentDir();

	// 레벨 파일 순회
	for (const auto& file : std::filesystem::recursive_directory_iterator(ContentDir + L"\\Level"))
	{
		if (file.is_regular_file())
		{
			std::wstring lvName = file.path().filename();
			lvName = lvName.substr(0, lvName.find(L'.'));
			std::wstring relativePath = L"Level\\" + lvName + L".level";
			CLevel* pLevel = CLevelMgr::GetInst()->FindLevel(relativePath);
			if (pLevel)
				pLevel->Save(lvName, true);
		}
	}

	// 에셋 파일 순회
	for (const auto& file : std::filesystem::recursive_directory_iterator(CPathMgr::GetContentDir()))
	{
		if (file.is_regular_file())
		{
			std::wstring fileName = file.path().filename();
			fileName = fileName.substr(0, fileName.find(L'.'));
			std::wstring rp = file.path().generic_wstring().substr(wstring(CPathMgr::GetContentDir()).size());
			for (int i = 0; i < rp.size(); ++i)
			{
				if (rp[i] == L'/')
					rp[i] = L'\\';
			}
			switch (CAssetMgr::GetInst()->GetAssetType(file.path()))
			{
			case ASSET_TYPE::PREFAB:
			{
				AssetPtr<CPrefab> pPrefab = CAssetMgr::GetInst()->Load<CPrefab>(rp);
				pPrefab->Save(fileName, true);
				break;
			}
			}

		}
	}
}
