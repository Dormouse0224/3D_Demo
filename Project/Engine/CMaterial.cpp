#include "pch.h"
#include "CMaterial.h"

#include "CDevice.h"
#include "CConstBuffer.h"
#include "CTexture.h"

#include "CPathMgr.h"
#include "CAssetMgr.h"
#include "CRenderMgr.h"

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

void CMaterial::SetMaterialCoefficient(Vec4 _vDiff, Vec4 _vSpec, Vec4 _vAmb, Vec4 _vEmis)

{
    m_Const.mtrl.vDiff = _vDiff;
    m_Const.mtrl.vAmb = _vAmb;
    m_Const.mtrl.vSpec = _vSpec;
    m_Const.mtrl.vEmv = _vEmis;
}

void CMaterial::Binding()
{
	m_Shader->Binding();

	for (UINT i = 0; i < TEX_PARAM::TEX_END; ++i)
	{
		if (nullptr != m_arrTex[i].Get())
		{
			m_arrTex[i]->Binding(i);
            m_Const.bTex[i] = true;
		}
        else
            m_Const.bTex[i] = false;
	}

	// ���� ������� ������ۿ� ���� �� ���ε�
	static CConstBuffer* pBuffer = CDevice::GetInst()->GetConstBuffer(CB_TYPE::MATERIAL);
	pBuffer->SetData(&m_Const, sizeof(tMtrlConst));
	pBuffer->Binding();
}

void CMaterial::Unbind()
{
    m_Shader->Unbind();

    for (UINT i = 0; i < TEX_PARAM::TEX_END; ++i)
    {
        if (nullptr != m_arrTex[i].Get())
        {
            m_arrTex[i]->Unbind(i);
            m_Const.bTex[i] = true;
        }
        else
            m_Const.bTex[i] = false;
    }

    // ���� ������� ������ۿ� ���� �� ���ε�
    static CConstBuffer* pBuffer = CDevice::GetInst()->GetConstBuffer(CB_TYPE::MATERIAL);
    pBuffer->Unbind();
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

	// ������ ����ϴ� �׷��� ���̴��� Ű��(�̸�) ����
	std::wstring ShaderName = L"";
	if (m_Shader.Get())
		ShaderName = m_Shader->GetName();
	int size = ShaderName.size();
	file.write(reinterpret_cast<char*>(&size), sizeof(int));
	file.write(reinterpret_cast<char*>(ShaderName.data()), sizeof(wchar_t) * size);

	// ������ ����ϴ� �ؽ����� Ű��(�̸�) ����
	for (int i = 0; i < TEX_END; ++i)
	{
		std::wstring TexName = {};
		if (m_arrTex[i].Get())
			TexName = m_arrTex[i]->GetName();
		int size = TexName.size();
		file.write(reinterpret_cast<char*>(&size), sizeof(int));
		file.write(reinterpret_cast<char*>(TexName.data()), sizeof(wchar_t) * size);
	}

	// ���� ��� ����ü ����
	file.write(reinterpret_cast<char*>(&m_Const), sizeof(tMtrlConst));

	file.close();
	return S_OK;
}

int CMaterial::Load(const wstring& _FilePath)
{
	std::filesystem::path path = CPathMgr::GetContentDir() + _FilePath;
	std::fstream file(path, std::ios::in | std::ios::binary);

	// ������ ����ϴ� �׷��� ���̴��� Ű��(�̸�) �ҷ�����
	std::wstring ShaderName = {};
	int size = 0;
	file.read(reinterpret_cast<char*>(&size), sizeof(int));
	ShaderName.resize(size);
	file.read(reinterpret_cast<char*>(ShaderName.data()), sizeof(wchar_t) * size);

	// �׷��� ���̴� �ε�
	if (!ShaderName.empty())
	{
		m_Shader = CAssetMgr::GetInst()->Load<CGraphicShader>(ShaderName);
		if (m_Shader == nullptr)
		{
			file.close();
			MessageBoxW(nullptr, L"�׷��� ���̴� �ε��� �����߽��ϴ�.", L"Material Load Error", MB_OK);
			return E_FAIL;
		}
	}

	// ������ ����ϴ� �ؽ����� Ű��(�̸�) �ҷ�����
	for (int i = 0; i < TEX_END; ++i)
	{
		std::wstring TexName = {};
		int size = TexName.size();
		file.read(reinterpret_cast<char*>(&size), sizeof(int));
		TexName.resize(size);
		file.read(reinterpret_cast<char*>(TexName.data()), sizeof(wchar_t) * size);

		// �ؽ��� �ε�
		if (!TexName.empty())
		{
			m_arrTex[i] = CAssetMgr::GetInst()->Load<CTexture>(TexName);
			if (m_arrTex[i] == nullptr)
			{
				file.close();
				MessageBoxW(nullptr, L"�ؽ��� �ε��� �����߽��ϴ�.", L"Material Load Error", MB_OK);
				return E_FAIL;
			}
		}
	}

	// ���� ��� ����ü �ҷ�����
	file.read(reinterpret_cast<char*>(&m_Const), sizeof(tMtrlConst));

	file.close();

	return S_OK;
}
