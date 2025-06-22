#include "pch.h"
#include "CLight.h"

#include "CLightMgr.h"
#include "CAssetMgr.h"
#include "CDevice.h"
#include "CMaterial.h"

CLight::CLight()
	: CComponent(COMPONENT_TYPE::LIGHT)
	, m_LightModule()
	, m_LightIdx(-1)
	, m_LightMesh(nullptr)
	, m_LightMtrl(nullptr)
{
	// Light Shader
	AssetPtr<CGraphicShader> pShader = new CGraphicShader;
	pShader->CreateVertexShader(L"HLSL\\Engine\\light.fx", "VS_Light");
	pShader->CreatePixelShader(L"HLSL\\Engine\\light.fx", "PS_Light");
	pShader->SetBSType(BS_TYPE::DEFAULT);
	pShader->SetRSType(RS_TYPE::CULL_NONE);
	pShader->SetDSType(DS_TYPE::NO_WRITE);
	pShader->SetDomain(SHADER_DOMAIN::DOMAIN_SYSTEM);
	pShader->SetEngineAsset(true);
	CAssetMgr::GetInst()->AddAsset(L"EA_LightShader", pShader.Get());

	// Light Material
	m_LightMtrl = new CMaterial;
	m_LightMtrl->SetName(L"EA_LightMtrl");
	m_LightMtrl->SetEngineAsset(true);
	m_LightMtrl->SetShader(CAssetMgr::GetInst()->Load<CGraphicShader>(L"EA_LightShader"));
	CAssetMgr::GetInst()->AddAsset(L"EA_LightMtrl", m_LightMtrl.Get());
}

CLight::CLight(const CLight& _Origin)
	: CComponent(_Origin)
	, m_LightModule(_Origin.m_LightModule)
	, m_LightIdx(-1)
	, m_LightMesh(_Origin.m_LightMesh)
	, m_LightMtrl(nullptr)
{
	AssetPtr<CMaterial> pMtrl = _Origin.m_LightMtrl;
	m_LightMtrl = pMtrl->Clone();
}

CLight::~CLight()
{
}

void CLight::FinalTick()
{
	CLightMgr::GetInst()->RegisterLight(this);
}

void CLight::Render()
{
	if (m_LightMesh == nullptr)
		return;

	//m_LightMtrl->SetTexParam(TEX_0, CDevice::GetInst()->GetRenderTarget(0));
	//m_LightMtrl->SetTexParam(TEX_1, CDevice::GetInst()->GetRenderTarget(1));
	//m_LightMtrl->SetTexParam(TEX_2, CDevice::GetInst()->GetRenderTarget(2));

	//m_LightMtrl->Binding();
	//m_LightMesh->Render();
}

void CLight::SetLightModule(tLightModule _Module)
{
	m_LightModule = _Module;
	if (_Module.Type == LIGHT_TYPE::DIRECTIONAL)
	{
		m_LightMesh = CAssetMgr::GetInst()->Load<CMesh>(L"EA_RectMesh", true);
	}
	else if (_Module.Type == LIGHT_TYPE::POINT)
	{
		m_LightMesh = CAssetMgr::GetInst()->Load<CMesh>(L"EA_SphereMesh", true);
	}
	else if (_Module.Type == LIGHT_TYPE::SPOT)
	{

	}
}

int CLight::Save(fstream& _Stream)
{
	_Stream.write((char*)&m_LightModule, sizeof(tLightModule));
	return 0;
}

int CLight::Load(fstream& _Stream)
{
	_Stream.read((char*)&m_LightModule, sizeof(tLightModule));
	return 0;
}
