#include "pch.h"
#include "CLight.h"

#include "CLightMgr.h"
#include "CAssetMgr.h"
#include "CRenderMgr.h"
#include "CMaterial.h"
#include "CMRT.h"
#include "CTransform.h"
#include "CCamera.h"
#include "CConstBuffer.h"
#include "CDevice.h"
#include "CFrustum.h"

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
    , m_CSM(true)
{
    m_LightMtrl = CAssetMgr::GetInst()->Load<CMaterial>(L"EA_LightMtrl", true);

    int resolution = 8192;
    m_DirectionalShadowMapMRT = new CMRT;
    m_DirectionalShadowMapMRT->CreateRenderTarget(CAssetMgr::GetInst()->CreateTexture(L"DirectionalShadowMapRT_" + to_wstring(GetID())
        , resolution, resolution, DXGI_FORMAT_R32_FLOAT, D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE, D3D11_USAGE_DEFAULT, 3, false));
    m_DirectionalShadowMapMRT->CreateDepthStencil(CAssetMgr::GetInst()->CreateTexture(L"DirectionalShadowMapDS_" + to_wstring(GetID())
        , resolution, resolution, DXGI_FORMAT_D24_UNORM_S8_UINT, D3D11_BIND_DEPTH_STENCIL, D3D11_USAGE_DEFAULT, 3, false));

    m_PointShadowMapMRT = new CMRT;
    m_PointShadowMapMRT->CreateRenderTarget(CAssetMgr::GetInst()->CreateTexture(L"PointShadowMapRT_" + to_wstring(GetID())
        , resolution / 8, resolution / 8, DXGI_FORMAT_R32_FLOAT, D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE, D3D11_USAGE_DEFAULT, 6, true));
    m_PointShadowMapMRT->CreateDepthStencil(CAssetMgr::GetInst()->CreateTexture(L"PointShadowMapDS_" + to_wstring(GetID())
        , resolution / 8, resolution / 8, DXGI_FORMAT_D24_UNORM_S8_UINT, D3D11_BIND_DEPTH_STENCIL, D3D11_USAGE_DEFAULT, 6, true));

    m_SpotShadowMapMRT = new CMRT;
    m_SpotShadowMapMRT->CreateRenderTarget(CAssetMgr::GetInst()->CreateTexture(L"SpotShadowMapRT_" + to_wstring(GetID())
        , resolution / 8, resolution / 8, DXGI_FORMAT_R32_FLOAT, D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE, D3D11_USAGE_DEFAULT, 1, false));
    m_SpotShadowMapMRT->CreateDepthStencil(CAssetMgr::GetInst()->CreateTexture(L"SpotShadowMapDS_" + to_wstring(GetID())
        , resolution / 8, resolution / 8, DXGI_FORMAT_D24_UNORM_S8_UINT, D3D11_BIND_DEPTH_STENCIL, D3D11_USAGE_DEFAULT, 1, false));

    m_LightCam = new CGameObject;
    m_LightCam->AddComponent(new CTransform);
    m_LightCam->AddComponent(new CCamera);
    m_LightCam->Camera()->SetViewX(resolution);
    m_LightCam->Camera()->SetViewY(resolution);
    m_LightCam->Camera()->CheckLayerAll();
    m_LightCam->Camera()->CheckLayer(LAYER::Background);
    m_LightCam->Camera()->CheckLayer(LAYER::UI);
    m_LightCam->Camera()->SetProjType(PROJ_TYPE::ORTHOGRAPHIC);
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
    , m_LightCam(_Origin.m_LightCam->Clone())
    , m_CSM(_Origin.m_CSM)
{
	AssetPtr<CMaterial> pMtrl = _Origin.m_LightMtrl;
	m_LightMtrl = pMtrl->Clone();

    int resolution = 8192;
    m_DirectionalShadowMapMRT = new CMRT;
    m_DirectionalShadowMapMRT->CreateRenderTarget(CAssetMgr::GetInst()->CreateTexture(L"DirectionalShadowMapRT_" + to_wstring(GetID())
        , resolution, resolution, DXGI_FORMAT_R32_FLOAT, D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE, D3D11_USAGE_DEFAULT, 3, false));
    m_DirectionalShadowMapMRT->CreateDepthStencil(CAssetMgr::GetInst()->CreateTexture(L"DirectionalShadowMapDS_" + to_wstring(GetID())
        , resolution, resolution, DXGI_FORMAT_D24_UNORM_S8_UINT, D3D11_BIND_DEPTH_STENCIL, D3D11_USAGE_DEFAULT, 3, false));

    m_PointShadowMapMRT = new CMRT;
    m_PointShadowMapMRT->CreateRenderTarget(CAssetMgr::GetInst()->CreateTexture(L"PointShadowMapRT_" + to_wstring(GetID())
        , resolution / 8, resolution / 8, DXGI_FORMAT_R32_FLOAT, D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE, D3D11_USAGE_DEFAULT, 6, true));
    m_PointShadowMapMRT->CreateDepthStencil(CAssetMgr::GetInst()->CreateTexture(L"PointShadowMapDS_" + to_wstring(GetID())
        , resolution / 8, resolution / 8, DXGI_FORMAT_D24_UNORM_S8_UINT, D3D11_BIND_DEPTH_STENCIL, D3D11_USAGE_DEFAULT, 6, true));

    m_SpotShadowMapMRT = new CMRT;
    m_SpotShadowMapMRT->CreateRenderTarget(CAssetMgr::GetInst()->CreateTexture(L"SpotShadowMapRT_" + to_wstring(GetID())
        , resolution / 8, resolution / 8, DXGI_FORMAT_R32_FLOAT, D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE, D3D11_USAGE_DEFAULT, 1, false));
    m_SpotShadowMapMRT->CreateDepthStencil(CAssetMgr::GetInst()->CreateTexture(L"SpotShadowMapDS_" + to_wstring(GetID())
        , resolution / 8, resolution / 8, DXGI_FORMAT_D24_UNORM_S8_UINT, D3D11_BIND_DEPTH_STENCIL, D3D11_USAGE_DEFAULT, 1, false));
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
    m_LightMtrl->SetConstParam(INT_1, (int)m_CSM);

    m_LightMtrl->SetTexParam(TEXARR_0, m_DirectionalShadowMapMRT->GetRenderTarget(0));
    m_LightMtrl->SetTexParam(TEXCUBE_0, m_PointShadowMapMRT->GetRenderTarget(0));
    m_LightMtrl->SetTexParam(TEX_2, m_SpotShadowMapMRT->GetRenderTarget(0));

    if (m_LightModule.Type == LIGHT_TYPE::DIRECTIONAL)
        m_LightMtrl->GetShader()->SetRSType(RS_TYPE::CULL_BACK);
    else
        m_LightMtrl->GetShader()->SetRSType(RS_TYPE::CULL_FRONT);

	m_LightMtrl->Binding();
	m_LightMesh->Render();

    m_LightMtrl->Unbind();
    CDevice::GetInst()->GetConstBuffer(CB_TYPE::SHADOW)->Unbind();
}

void CLight::ShadowRender()
{
    if (m_LightModule.Type == LIGHT_TYPE::DIRECTIONAL)
    {
        m_DirectionalShadowMapMRT->ClearTargets();
        m_DirectionalShadowMapMRT->SetRenderTarget();
    }
    else if (m_LightModule.Type == LIGHT_TYPE::POINT)
    {
        m_PointShadowMapMRT->ClearTargets();
        m_PointShadowMapMRT->SetRenderTarget();
    }
    else if (m_LightModule.Type == LIGHT_TYPE::SPOT)
    {
        m_SpotShadowMapMRT->ClearTargets();
        m_SpotShadowMapMRT->SetRenderTarget();
    }

    SetMatrixShadow();
    m_LightCam->Camera()->RenderShadow(m_LightModule.Type, m_CSM);
}

void CLight::SetMatrixShadow()
{
    m_ShadowMat = {};

    if (m_LightModule.Type == LIGHT_TYPE::DIRECTIONAL)
    {
        if (m_CSM)
        {
            m_ShadowMat.matVP_0 = CalcDirectionalVP(CRenderMgr::GetInst()->GetCurrentCam()->GetFrustum()->GetCircumscribedSphere(CASCADE::eNEAR), m_LightModule.Dir);
            m_ShadowMat.matVP_1 = CalcDirectionalVP(CRenderMgr::GetInst()->GetCurrentCam()->GetFrustum()->GetCircumscribedSphere(CASCADE::eMIDDLE), m_LightModule.Dir);
            m_ShadowMat.matVP_2 = CalcDirectionalVP(CRenderMgr::GetInst()->GetCurrentCam()->GetFrustum()->GetCircumscribedSphere(CASCADE::eFAR), m_LightModule.Dir);
        }
        else
        {
            m_ShadowMat.matVP_0 = CalcDirectionalVP(CRenderMgr::GetInst()->GetCurrentCam()->GetFrustum()->GetCircumscribedSphere(CASCADE::eAll), m_LightModule.Dir);
        }
    }
    else if (m_LightModule.Type == LIGHT_TYPE::POINT)
    {
        Vec3 vR = Transform()->GetRelativeDir(DIR::RIGHT);
        Vec3 vU = Transform()->GetRelativeDir(DIR::UP);
        Vec3 vF = Transform()->GetRelativeDir(DIR::FRONT);
        m_ShadowMat.matVP_0 = CalcPointVP(vR, vU);
        m_ShadowMat.matVP_1 = CalcPointVP(-vR, vU);
        m_ShadowMat.matVP_2 = CalcPointVP(vU, -vF);
        m_ShadowMat.matVP_3 = CalcPointVP(-vU, vF);
        m_ShadowMat.matVP_4 = CalcPointVP(vF, vU);
        m_ShadowMat.matVP_5 = CalcPointVP(-vF, vU);
    }
    else if (m_LightModule.Type == LIGHT_TYPE::SPOT)
    {
        m_ShadowMat.matVP_0 = CalcSpotlightVP();
    }

    CConstBuffer* pBuffer = CDevice::GetInst()->GetConstBuffer(CB_TYPE::SHADOW);
    pBuffer->SetData(&m_ShadowMat, sizeof(tShadowMat));
    pBuffer->Binding();
}

Matrix CLight::CalcDirectionalVP(Vec4 _CircumscribedSphere, Vec3 _LightDir)
{
    Vec3 NormDir = _LightDir;
    NormDir.Normalize();
    float fFar = 10000.f;
    float fViewXY = _CircumscribedSphere.w * 2.f;

    // 뷰행렬 계산
    // 이동
    Vec3 center = _CircumscribedSphere;
    Vec3 vCamPos = center - NormDir * (fFar - _CircumscribedSphere.w);
    Matrix matTrans = XMMatrixTranslation(-vCamPos.x, -vCamPos.y, -vCamPos.z);

    // 회전
    Vec3 vR = NormDir.Cross(Vec3(0.f, 1.f, 0.f));
    Vec3 vU = vR.Cross(NormDir);
    Vec3 vF = NormDir;
    vR.Normalize(); vU.Normalize(); vF.Normalize();

    Matrix matView = XMMatrixLookToLH(vCamPos, vF, vU);

    // 투영행렬 계산하기
    Matrix matProj = XMMatrixOrthographicLH(fViewXY, fViewXY, 1.f, fFar);
    
    return matView * matProj;
}

Matrix CLight::CalcPointVP(Vec3 _LightDir, Vec3 _UpDir)
{
    if (m_LightModule.Radius < 2.f)
        m_LightModule.Radius = 2.f;
    float fFar = m_LightModule.Radius;

    // 뷰행렬 계산
    // 이동
    Vec3 vCamPos = Transform()->GetWorldTrans();

    // 회전
    Vec3 vU = _UpDir;
    Vec3 vF = _LightDir;
    vU.Normalize(); vF.Normalize();

    Matrix matView = XMMatrixLookToLH(vCamPos, vF, vU);

    // 투영행렬 계산하기
    Matrix matProj = XMMatrixPerspectiveFovLH(0.5f * XM_PI, 1.f, 1.f, fFar);

    return matView * matProj;
}

Matrix CLight::CalcSpotlightVP()
{
    if (m_LightModule.Radius < 2.f)
        m_LightModule.Radius = 2.f;
    float fFar = m_LightModule.Radius;

    // 뷰행렬 계산
    // 이동
    Vec3 vCamPos = Transform()->GetWorldTrans();

    // 회전
    Vec3 vR = Transform()->GetRelativeDir(DIR::RIGHT);
    Vec3 vU = Transform()->GetRelativeDir(DIR::UP);
    Vec3 vF = Transform()->GetRelativeDir(DIR::FRONT);
    vR.Normalize(); vU.Normalize(); vF.Normalize();

    Matrix matView = XMMatrixLookToLH(vCamPos, vF, vU);

    // 투영행렬 계산하기
    if (m_LightModule.Angle < 1.f)
        m_LightModule.Angle = 1.f;
    Matrix matProj = XMMatrixPerspectiveFovLH(m_LightModule.Angle * XM_PI / 180.f, 1.f, 1.f, fFar);

    return matView * matProj;
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
