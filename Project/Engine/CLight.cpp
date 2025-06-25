#include "pch.h"
#include "CLight.h"

#include "CLightMgr.h"
#include "CAssetMgr.h"
#include "CRenderMgr.h"
#include "CMaterial.h"
#include "CMRT.h"

CLight::CLight()
	: CComponent(COMPONENT_TYPE::LIGHT)
	, m_LightModule()
	, m_LightIdx(-1)
	, m_LightMesh(nullptr)
	, m_LightMtrl(nullptr)
{
    m_LightMtrl = CAssetMgr::GetInst()->Load<CMaterial>(L"EA_LightMtrl", true);
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

    // Normal, Position 타겟을 리소스로 바인딩
	m_LightMtrl->SetTexParam(TEX_0, CRenderMgr::GetInst()->GetMRT(MRT_TYPE::DEFERRED)->GetRenderTarget(1));
	m_LightMtrl->SetTexParam(TEX_1, CRenderMgr::GetInst()->GetMRT(MRT_TYPE::DEFERRED)->GetRenderTarget(2));
    m_LightMtrl->SetConstParam(INT_0, m_LightIdx);

	m_LightMtrl->Binding();
	m_LightMesh->Render();
}

void CLight::SetLightModule(tLightModule _Module)
{
	m_LightModule = _Module;
	if (_Module.Type == LIGHT_TYPE::DIRECTIONAL)
	{
        // 화면 전체 픽셀셰이더 호출을 위해 사각형 메시를 사용
		m_LightMesh = CAssetMgr::GetInst()->Load<CMesh>(L"EA_RectMesh", true);
	}
	else if (_Module.Type == LIGHT_TYPE::POINT)
	{
        // 점광원은 범위안의 픽셀만 호출하므로 구형 메시를 사용
		m_LightMesh = CAssetMgr::GetInst()->Load<CMesh>(L"EA_SphereMesh", true);
	}
	else if (_Module.Type == LIGHT_TYPE::SPOT)
	{

	}
}

int CLight::Save(fstream& _Stream)
{
	_Stream.write((char*)&m_LightModule, sizeof(tLightModule));

	return S_OK;
}

int CLight::Load(fstream& _Stream)
{
    tLightModule lightModule;
	_Stream.read((char*)&lightModule, sizeof(tLightModule));
    SetLightModule(lightModule);

    return S_OK;
}
