#include "pch.h"
#include "CMaterial.h"

#include "CDevice.h"
#include "CConstBuffer.h"
#include "CTexture2D.h"

#include "CPathMgr.h"
#include "CAssetMgr.h"

CMaterial::CMaterial()
	: CAsset(ASSET_TYPE::MATERIAL)
	, m_Shader(nullptr)
	, m_arrTex{}
	, m_Const{}
{

}

CMaterial::CMaterial(const CMaterial& _Other)
	: CAsset(_Other)
	, m_Shader(_Other.m_Shader)
	, m_arrTex{}
	, m_Const{ _Other.m_Const }
{
	for (int i = 0; i < TEX_END; ++i)
	{
		SetTexParam(static_cast<TEX_PARAM>(i), _Other.m_arrTex[i]);
	}
}

CMaterial::~CMaterial()
{
}

void CMaterial::Binding()
{
	m_Shader->Binding();

	for (UINT i = 0; i < TEX_PARAM::TEX_END; ++i)
	{
		if (nullptr != m_arrTex[i].Get())
		{
			m_arrTex[i]->Binding(i);
			AssetPtr<CTexture2D> pNormTex = CAssetMgr::GetInst()->GetNormTex(m_arrTex[i]->GetName());
			CTexture2D::Unbind(TEX_PARAM::TEX_END + i);
			if (pNormTex.Get())
				pNormTex->Binding(TEX_PARAM::TEX_END + i);
		}
	}

	// 재질 상수값을 상수버퍼에 전달 및 바인딩
	static CConstBuffer* pBuffer = CDevice::GetInst()->GetConstBuffer(CB_TYPE::MATERIAL);
	pBuffer->SetData(&m_Const, sizeof(tMtrlConst));
	pBuffer->Binding();
}

AssetPtr<CMaterial> CMaterial::Create(wstring _Name)
{
	AssetPtr<CMaterial> pNewMtrl = new CMaterial;
	pNewMtrl->SetName(_Name);
	pNewMtrl->Save(_Name);
	return pNewMtrl;
}

int CMaterial::Save(const wstring& _FileName, bool _Update)
{
	std::filesystem::path path = CPathMgr::GetContentDir() + std::wstring(L"Material\\") + _FileName + std::wstring(L".mtrl");
	CPathMgr::CreateParentDir(path);
	std::fstream file(path, std::ios::out | std::ios::binary);

	// 재질이 사용하는 그래픽 셰이더의 키값(이름) 저장
	std::wstring ShaderName = L"";
	if (m_Shader.Get())
		ShaderName = m_Shader->GetName();
	int size = ShaderName.size();
	file.write(reinterpret_cast<char*>(&size), sizeof(int));
	file.write(reinterpret_cast<char*>(ShaderName.data()), sizeof(wchar_t) * size);

	// 재질이 사용하는 텍스쳐의 키값(이름) 저장
	for (int i = 0; i < TEX_END; ++i)
	{
		std::wstring TexName = {};
		if (m_arrTex[i].Get())
			TexName = m_arrTex[i]->GetName();
		int size = TexName.size();
		file.write(reinterpret_cast<char*>(&size), sizeof(int));
		file.write(reinterpret_cast<char*>(TexName.data()), sizeof(wchar_t) * size);
	}

	// 재질 상수 구조체 저장
	file.write(reinterpret_cast<char*>(&m_Const), sizeof(tMtrlConst));

	file.close();
	return S_OK;
}

int CMaterial::Load(const wstring& _FilePath)
{
	std::filesystem::path path = CPathMgr::GetContentDir() + _FilePath;
	std::fstream file(path, std::ios::in | std::ios::binary);

	// 재질이 사용하는 그래픽 셰이더의 키값(이름) 불러오기
	std::wstring ShaderName = {};
	int size = 0;
	file.read(reinterpret_cast<char*>(&size), sizeof(int));
	ShaderName.resize(size);
	file.read(reinterpret_cast<char*>(ShaderName.data()), sizeof(wchar_t) * size);

	// 그래픽 셰이더 로딩
	if (!ShaderName.empty())
	{
		m_Shader = CAssetMgr::GetInst()->Load<CGraphicShader>(ShaderName);
		if (m_Shader == nullptr)
		{
			file.close();
			MessageBoxW(nullptr, L"그래픽 셰이더 로딩에 실패했습니다.", L"Material Load Error", MB_OK);
			return E_FAIL;
		}
	}

	// 재질이 사용하는 텍스쳐의 키값(이름) 불러오기
	for (int i = 0; i < TEX_END; ++i)
	{
		std::wstring TexName = {};
		int size = TexName.size();
		file.read(reinterpret_cast<char*>(&size), sizeof(int));
		TexName.resize(size);
		file.read(reinterpret_cast<char*>(TexName.data()), sizeof(wchar_t) * size);

		// 텍스쳐 로딩
		if (!TexName.empty())
		{
			m_arrTex[i] = CAssetMgr::GetInst()->Load<CTexture2D>(TexName);
			if (m_arrTex[i] == nullptr)
			{
				file.close();
				MessageBoxW(nullptr, L"텍스쳐 로딩에 실패했습니다.", L"Material Load Error", MB_OK);
				return E_FAIL;
			}
		}
	}

	// 재질 상수 구조체 불러오기
	file.read(reinterpret_cast<char*>(&m_Const), sizeof(tMtrlConst));

	file.close();

	return S_OK;
}
