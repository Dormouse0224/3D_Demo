#include "pch.h"
#include "CRenderMgr.h"

#include "CGameObject.h"
#include "components.h"
#include "CDevice.h"
#include "CMRT.h"

#include "CAssetMgr.h"
#include "assets.h"
#include "CEditorCamScript.h"

void CRenderMgr::Init()
{
	// ================
	// DebugShapeShader
	// ================
	AssetPtr<CGraphicShader> pShader = new CGraphicShader;

	pShader->CreateVertexShader(L"HLSL\\Engine\\debug_shape.fx", "VS_DebugShape");
	pShader->CreatePixelShader(L"HLSL\\Engine\\debug_shape.fx", "PS_DebugShape");
	pShader->SetDomain(SHADER_DOMAIN::DOMAIN_DEBUG);
	pShader->SetRSType(RS_TYPE::CULL_NONE);
	pShader->SetDSType(DS_TYPE::LESS);
	pShader->SetBSType(BS_TYPE::ALPHABLEND);
	pShader->SetTopology(D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP);
	pShader->SetEngineAsset(true);

	CAssetMgr::GetInst()->AddAsset(L"EA_DebugShapeShader", pShader.Get());

	// ==============
	// DebugShapeMtrl
	// ==============
	AssetPtr<CMaterial> pMtrl = new CMaterial;
	pMtrl->SetShader(pShader);
	pMtrl->SetEngineAsset(true);
	CAssetMgr::GetInst()->AddAsset(L"EA_DebugShapeMtrl", pMtrl.Get());


	// ==================
	// DebugRender Object
	// ==================
	m_DebugObject = new CGameObject;
	m_DebugObject->AddComponent(new CTransform);
	m_DebugObject->AddComponent(new CMeshRender);
	m_DebugObject->MeshRender()->SetMaterial(pMtrl);


	// ==================
	// Editor Cam Object
	// ==================
	m_EditorCam = new CGameObject;
	m_EditorCam->AddComponent(new CTransform);
	m_EditorCam->AddComponent(new CCamera);
	m_EditorCam->AddComponent(new CEditorCamScript);
	m_EditorCam->Camera()->SetProjType(PROJ_TYPE::PERSPECTIVE);
	m_EditorCam->Camera()->CheckLayerAll();
	m_EditorCam->Camera()->CheckLayer(LAYER::UI);


	// ==================
	// UI Cam Object
	// ==================
	m_UICam = new CGameObject;
	m_UICam->AddComponent(new CTransform);
	m_UICam->AddComponent(new CCamera);
	m_UICam->Camera()->SetProjType(PROJ_TYPE::ORTHOGRAPHIC);
	m_UICam->Camera()->CheckLayer(LAYER::UI);

	// Merge Mesh
	m_MergeMesh = CAssetMgr::GetInst()->Load<CMesh>(L"EA_RectMesh", true);

	// Merge Shader
	pShader = new CGraphicShader;
	pShader->CreateVertexShader(L"HLSL\\Engine\\merge.fx", "VS_Merge");
	pShader->CreatePixelShader(L"HLSL\\Engine\\merge.fx", "PS_Merge");
	pShader->SetBSType(BS_TYPE::DEFAULT);
	pShader->SetRSType(RS_TYPE::CULL_NONE);
	pShader->SetDSType(DS_TYPE::NO_WRITE);
	pShader->SetDomain(SHADER_DOMAIN::DOMAIN_SYSTEM);
	pShader->SetEngineAsset(true);
	CAssetMgr::GetInst()->AddAsset(L"EA_MergeShader", pShader.Get());

	// Merge Material
	m_MergeMtrl = new CMaterial;
	m_MergeMtrl->SetName(L"EA_MergeMtrl");
	m_MergeMtrl->SetEngineAsset(true);
	m_MergeMtrl->SetShader(CAssetMgr::GetInst()->Load<CGraphicShader>(L"EA_MergeShader"));
	CAssetMgr::GetInst()->AddAsset(L"EA_MergeMtrl", m_MergeMtrl.Get());

	CreateBackBufferView();
	CreateMRT();
}

int CRenderMgr::CreateBackBufferView()
{
	m_arrMRT[MRT_TYPE::MERGE] = new CMRT;
	m_arrMRT[MRT_TYPE::MERGE]->SetViewport(CDevice::GetInst()->GetViewPort());

	// RenderTargetView 생성
	// 이미 생성되어있는 렌더타겟 텍스쳐를 스왚체인으로 부터 얻어온다.
	ComPtr<ID3D11Texture2D> pRTTex = nullptr;
	HRESULT hr = CDevice::GetInst()->GetSwapChain()->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)pRTTex.GetAddressOf());

	m_arrMRT[MRT_TYPE::MERGE]->CreateRenderTarget(pRTTex, L"MergeRTT");

	// DepthStencil 리소스(텍스쳐) 생성    
	D3D11_TEXTURE2D_DESC DSTexDesc = {};

	DSTexDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	DSTexDesc.ArraySize = 1;
	DSTexDesc.Width = (UINT)CDevice::GetInst()->GetRenderResolution().x;
	DSTexDesc.Height = (UINT)CDevice::GetInst()->GetRenderResolution().y;
	DSTexDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

	// System Memroy 이동 불가능
	DSTexDesc.Usage = D3D11_USAGE_DEFAULT;
	DSTexDesc.CPUAccessFlags = 0;

	DSTexDesc.MipLevels = 1;
	DSTexDesc.SampleDesc.Count = 1;
	DSTexDesc.SampleDesc.Quality = 0;

	m_arrMRT[MRT_TYPE::MERGE]->CreateDepthStencil(DSTexDesc, L"MergeDST");

	return S_OK;
}

int CRenderMgr::CreateMRT()
{
	m_arrMRT[MRT_TYPE::DEFERRED] = new CMRT;
	m_arrMRT[MRT_TYPE::DEFERRED]->SetViewport(CDevice::GetInst()->GetViewPort());

	// Deffered Render Target 텍스쳐 생성
	D3D11_TEXTURE2D_DESC texDesc = {};
	texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	texDesc.ArraySize = 1;
	texDesc.Width = (UINT)CDevice::GetInst()->GetRenderResolution().x;
	texDesc.Height = (UINT)CDevice::GetInst()->GetRenderResolution().y;
	texDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;

	// System Memroy 이동 불가능
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.CPUAccessFlags = 0;

	texDesc.MipLevels = 1;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;

	m_arrMRT[MRT_TYPE::DEFERRED]->CreateRenderTarget(texDesc, L"DeferredRTT_Albedo");

	texDesc = {};
	texDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	texDesc.ArraySize = 1;
	texDesc.Width = (UINT)CDevice::GetInst()->GetRenderResolution().x;
	texDesc.Height = (UINT)CDevice::GetInst()->GetRenderResolution().y;
	texDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;

	// System Memroy 이동 불가능
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.CPUAccessFlags = 0;

	texDesc.MipLevels = 1;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;

	m_arrMRT[MRT_TYPE::DEFERRED]->CreateRenderTarget(texDesc, L"DeferredRTT_Normal");

	texDesc = {};
	texDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	texDesc.ArraySize = 1;
	texDesc.Width = (UINT)CDevice::GetInst()->GetRenderResolution().x;
	texDesc.Height = (UINT)CDevice::GetInst()->GetRenderResolution().y;
	texDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;

	// System Memroy 이동 불가능
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.CPUAccessFlags = 0;

	texDesc.MipLevels = 1;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;

	m_arrMRT[MRT_TYPE::DEFERRED]->CreateRenderTarget(texDesc, L"DeferredRTT_Position");

	// MRT 의 DSV 텍스쳐 생성
	D3D11_TEXTURE2D_DESC DSTexDesc = {};
	DSTexDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	DSTexDesc.ArraySize = 1;
	DSTexDesc.Width = (UINT)CDevice::GetInst()->GetRenderResolution().x;
	DSTexDesc.Height = (UINT)CDevice::GetInst()->GetRenderResolution().y;
	DSTexDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

	// System Memroy 이동 불가능
	DSTexDesc.Usage = D3D11_USAGE_DEFAULT;
	DSTexDesc.CPUAccessFlags = 0;

	DSTexDesc.MipLevels = 1;
	DSTexDesc.SampleDesc.Count = 1;
	DSTexDesc.SampleDesc.Quality = 0;

	m_arrMRT[MRT_TYPE::DEFERRED]->CreateDepthStencil(DSTexDesc, L"DeferredDST");

	return S_OK;
}