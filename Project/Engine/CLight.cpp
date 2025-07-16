#include "pch.h"
#include "CLight.h"

#include "CLightMgr.h"
#include "CAssetMgr.h"
#include "CRenderMgr.h"
#include "CMaterial.h"
#include "CMRT.h"
#include "CTransform.h"
#include "CCamera.h"

CLight::CLight()
	: CComponent(COMPONENT_TYPE::LIGHT)
	, m_LightModule()
	, m_LightIdx(-1)
	, m_LightMesh(nullptr)
	, m_LightMtrl(nullptr)
    , m_DirectionalShadowMapMRT(nullptr)
    , m_PointShadowMapMRT(nullptr)
    , m_SpotShadowMapMRT(nullptr)
    , m_LightCam(nullptr)
{
    m_LightMtrl = CAssetMgr::GetInst()->Load<CMaterial>(L"EA_LightMtrl", true);

    m_DirectionalShadowMapMRT = new CMRT;
    m_DirectionalShadowMapMRT->CreateRenderTarget(CAssetMgr::GetInst()->CreateTexture(L"DirectionalShadowMapRT_" + to_wstring(GetID())
        , 4096, 4096, DXGI_FORMAT_R32_FLOAT, D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE, D3D11_USAGE_DEFAULT, 3, false));
    m_DirectionalShadowMapMRT->CreateDepthStencil(CAssetMgr::GetInst()->CreateTexture(L"DirectionalShadowMapDS_" + to_wstring(GetID())
        , 4096, 4096, DXGI_FORMAT_D24_UNORM_S8_UINT, D3D11_BIND_DEPTH_STENCIL, D3D11_USAGE_DEFAULT, 3, false));

    m_PointShadowMapMRT = new CMRT;
    m_PointShadowMapMRT->CreateRenderTarget(CAssetMgr::GetInst()->CreateTexture(L"PointShadowMapRT_" + to_wstring(GetID())
        , 4096, 4096, DXGI_FORMAT_R32_FLOAT, D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE, D3D11_USAGE_DEFAULT, 3, false));
    m_PointShadowMapMRT->CreateDepthStencil(CAssetMgr::GetInst()->CreateTexture(L"PointShadowMapDS_" + to_wstring(GetID())
        , 4096, 4096, DXGI_FORMAT_D24_UNORM_S8_UINT, D3D11_BIND_DEPTH_STENCIL, D3D11_USAGE_DEFAULT, 3, false));

    m_SpotShadowMapMRT = new CMRT;
    m_SpotShadowMapMRT->CreateRenderTarget(CAssetMgr::GetInst()->CreateTexture(L"SpotShadowMapRT_" + to_wstring(GetID())
        , 4096, 4096, DXGI_FORMAT_R32_FLOAT, D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE, D3D11_USAGE_DEFAULT, 3, false));
    m_SpotShadowMapMRT->CreateDepthStencil(CAssetMgr::GetInst()->CreateTexture(L"SpotShadowMapDS_" + to_wstring(GetID())
        , 4096, 4096, DXGI_FORMAT_D24_UNORM_S8_UINT, D3D11_BIND_DEPTH_STENCIL, D3D11_USAGE_DEFAULT, 3, false));

    m_LightCam = new CGameObject;
    m_LightCam->AddComponent(new CTransform);
    m_LightCam->AddComponent(new CCamera);
    m_LightCam->Camera()->SetViewX(4096);
    m_LightCam->Camera()->SetViewY(4096);
}

CLight::CLight(const CLight& _Origin)
	: CComponent(_Origin)
	, m_LightModule(_Origin.m_LightModule)
	, m_LightIdx(-1)
	, m_LightMesh(_Origin.m_LightMesh)
	, m_LightMtrl(nullptr)
    , m_DirectionalShadowMapMRT(nullptr)
    , m_PointShadowMapMRT(nullptr)
    , m_SpotShadowMapMRT(nullptr)
    , m_LightCam(nullptr)
{
	AssetPtr<CMaterial> pMtrl = _Origin.m_LightMtrl;
	m_LightMtrl = pMtrl->Clone();

    m_DirectionalShadowMapMRT = new CMRT;
    m_DirectionalShadowMapMRT->CreateRenderTarget(CAssetMgr::GetInst()->CreateTexture(L"DirectionalShadowMapRT_" + to_wstring(GetID())
        , 4096, 4096, DXGI_FORMAT_R32_FLOAT, D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE, D3D11_USAGE_DEFAULT, 3, false));
    m_DirectionalShadowMapMRT->CreateDepthStencil(CAssetMgr::GetInst()->CreateTexture(L"DirectionalShadowMapDS_" + to_wstring(GetID())
        , 4096, 4096, DXGI_FORMAT_D24_UNORM_S8_UINT, D3D11_BIND_DEPTH_STENCIL, D3D11_USAGE_DEFAULT, 3, false));

    m_PointShadowMapMRT = new CMRT;
    m_PointShadowMapMRT->CreateRenderTarget(CAssetMgr::GetInst()->CreateTexture(L"PointShadowMapRT_" + to_wstring(GetID())
        , 4096, 4096, DXGI_FORMAT_R32_FLOAT, D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE, D3D11_USAGE_DEFAULT, 3, false));
    m_PointShadowMapMRT->CreateDepthStencil(CAssetMgr::GetInst()->CreateTexture(L"PointShadowMapDS_" + to_wstring(GetID())
        , 4096, 4096, DXGI_FORMAT_D24_UNORM_S8_UINT, D3D11_BIND_DEPTH_STENCIL, D3D11_USAGE_DEFAULT, 3, false));

    m_SpotShadowMapMRT = new CMRT;
    m_SpotShadowMapMRT->CreateRenderTarget(CAssetMgr::GetInst()->CreateTexture(L"SpotShadowMapRT_" + to_wstring(GetID())
        , 4096, 4096, DXGI_FORMAT_R32_FLOAT, D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE, D3D11_USAGE_DEFAULT, 3, false));
    m_SpotShadowMapMRT->CreateDepthStencil(CAssetMgr::GetInst()->CreateTexture(L"SpotShadowMapDS_" + to_wstring(GetID())
        , 4096, 4096, DXGI_FORMAT_D24_UNORM_S8_UINT, D3D11_BIND_DEPTH_STENCIL, D3D11_USAGE_DEFAULT, 3, false));

    m_LightCam = new CGameObject;
    m_LightCam->AddComponent(new CTransform);
    m_LightCam->AddComponent(new CCamera);
    m_LightCam->Camera()->SetViewX(256);
    m_LightCam->Camera()->SetViewY(256);
    m_LightCam->Camera()->CheckLayerAll();
    m_LightCam->Camera()->CheckLayer(LAYER::UI);
    m_LightCam->Camera()->SetProjType(PROJ_TYPE::ORTHOGRAPHIC);
}

CLight::~CLight()
{
    delete m_DirectionalShadowMapMRT;
    delete m_PointShadowMapMRT;
    delete m_SpotShadowMapMRT;
    delete m_LightCam;
}

void CLight::FinalTick()
{
    m_LightModule.WorldPos = Transform()->GetWorldTrans();
    m_LightModule.Dir = Transform()->GetWorldDir(DIR::FRONT);
    Transform()->SetRelativeScale(Vec3(m_LightModule.Radius * 2));
	CLightMgr::GetInst()->RegisterLight(this);

    if (m_LightModule.Type == LIGHT_TYPE::DIRECTIONAL)
    {
        DrawDebugLine(Vec4(0.f, 1.f, 0.f, 1.f), m_LightModule.WorldPos, m_LightModule.Dir, m_LightModule.Radius, false, 0.f);
        DrawDebugSphere(Vec4(0.f, 1.f, 0.f, 1.f), m_LightModule.WorldPos, 1.f, false, 0.f);
    }
    else if (m_LightModule.Type == LIGHT_TYPE::POINT)
    {
        DrawDebugSphere(Vec4(0.f, 1.f, 0.f, 1.f), m_LightModule.WorldPos, m_LightModule.Radius, false, 0.f);
    }
    else if (m_LightModule.Type == LIGHT_TYPE::SPOT)
    {
        DrawDebugLine(Vec4(0.f, 1.f, 0.f, 1.f), m_LightModule.WorldPos, m_LightModule.Dir, m_LightModule.Radius, false, 0.f);
        DrawDebugSphere(Vec4(0.f, 1.f, 0.f, 1.f), m_LightModule.WorldPos, m_LightModule.Radius, false, 0.f);
    }

    m_LightCam->Transform()->SetRelativePos(Transform()->GetRelativePos());
    m_LightCam->Transform()->SetRelativeRot(Transform()->GetRelativeRotQuat());
    m_LightCam->FinalTick();
}

void CLight::Render()
{
	if (m_LightMesh == nullptr)
		return;

    ShadowRender();

    // 출력 렌더타겟 및 출력 깊이타겟 설정
    CRenderMgr::GetInst()->GetMRT(MRT_TYPE::LIGHT)->SetRenderTarget();

    Transform()->Binding();

    CRenderMgr::GetInst()->GetCurrentCam()->SetMatrix();

    // Normal, Position 타겟을 리소스로 바인딩
	m_LightMtrl->SetTexParam(TEX_0, CRenderMgr::GetInst()->GetMRT(MRT_TYPE::DEFERRED)->GetRenderTarget(1));
	m_LightMtrl->SetTexParam(TEX_1, CRenderMgr::GetInst()->GetMRT(MRT_TYPE::DEFERRED)->GetRenderTarget(2));
    m_LightMtrl->SetConstParam(INT_0, m_LightIdx);

	m_LightMtrl->Binding();
	m_LightMesh->Render();
}

void CLight::ShadowRender()
{
    m_DirectionalShadowMapMRT->ClearTargets();
    m_DirectionalShadowMapMRT->SetRenderTarget();
    
    m_LightCam->Camera()->RenderShadow();

    CRenderMgr::GetInst()->GetCurrentCam()->SetMatrix();
}

void CLight::SetLightModule(tLightModule _Module)
{
	m_LightModule = _Module;
	if (_Module.Type == LIGHT_TYPE::DIRECTIONAL)
	{
        // 화면 전체 픽셀셰이더 호출을 위해 사각형 메시를 사용
		m_LightMesh = CAssetMgr::GetInst()->Load<CMesh>(L"EA_RectMesh", true);
	}
	else if (_Module.Type == LIGHT_TYPE::POINT || _Module.Type == LIGHT_TYPE::SPOT)
	{
        // 점광원은 범위안의 픽셀만 호출하므로 구형 메시를 사용
		m_LightMesh = CAssetMgr::GetInst()->Load<CMesh>(L"EA_SphereMesh", true);
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
