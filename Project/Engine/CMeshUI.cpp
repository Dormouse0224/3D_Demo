#include "pch.h"
#include "CMeshUI.h"
#include "CMesh.h"

#include "CAssetMgr.h"
#include "CTaskMgr.h"
#include "CKeyMgr.h"
#include "CTimeMgr.h"

#include "CGameObject.h"
#include "CTransform.h"
#include "CCamera.h"
#include "CTexture2D.h"
#include "CDevice.h"
#include "CMeshRender.h"

CMeshUI::CMeshUI()
	: CAssetUI(ASSET_TYPE_WSTR[(UINT)ASSET_TYPE::MESH])
	, m_ViewPort{}
{
	m_VertexRTTex = CAssetMgr::GetInst()->CreateTexture(L"VertexRTTex", 300, 300, DXGI_FORMAT_R8G8B8A8_UNORM, D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE);
	m_VertexDSTex = CAssetMgr::GetInst()->CreateTexture(L"VertexDSTex", 300, 300, DXGI_FORMAT_D24_UNORM_S8_UINT, D3D11_BIND_DEPTH_STENCIL);

	// ViewPort 설정
	m_ViewPort.TopLeftX = 0;
	m_ViewPort.TopLeftY = 0;
	m_ViewPort.Width = 300;
	m_ViewPort.Height = 300;
	m_ViewPort.MinDepth = 0.f;    // 깊이 텍스쳐에 저장하는 깊이값의 범위
	m_ViewPort.MaxDepth = 1.f;

	m_MeshCam = new CGameObject;
	m_MeshCam->AddComponent(new CTransform);
	m_MeshCam->AddComponent(new CCamera);
	m_MeshCam->Camera()->SetProjType(PROJ_TYPE::PERSPECTIVE);
	m_MeshCam->Camera()->SetViewX(300);
	m_MeshCam->Camera()->SetViewY(300);


	m_VertexObject = new CGameObject;
	m_VertexObject->AddComponent(new CTransform);
	m_VertexObject->AddComponent(new CMeshRender);
	m_VertexObject->MeshRender()->SetMaterial(CAssetMgr::GetInst()->Load<CMaterial>(L"EA_DebugShapeMtrl", true));
	m_VertexObject->Transform()->SetRelativePos(0, 0, 1);
}

CMeshUI::~CMeshUI()
{
	delete m_MeshCam;
	delete m_VertexObject;
}

void CMeshUI::Update_Ast()
{
	m_VertexObject->MeshRender()->SetMesh(static_cast<CMesh*>(m_TargetAsset.Get()));
}

void CMeshUI::Render_Ast()
{
	float tab = 130.f;

	AssetPtr<CMesh> pMesh = static_cast<CMesh*>(m_TargetAsset.Get());
	int VtxCount = pMesh->GetVtxCount();
	int IdxCount = pMesh->GetIdxCount();

	ImGui::Text("Vertex Count");
	ImGui::SameLine(tab);
	ImGui::BeginDisabled(true);
	ImGui::InputInt("##VertexCount", &VtxCount, 0, 0, ImGuiInputTextFlags_ReadOnly);
	ImGui::EndDisabled();

	ImGui::Text("Index Count");
	ImGui::SameLine(tab);
	ImGui::BeginDisabled(true);
	ImGui::InputInt("##IndexCount", &IdxCount, 0, 0, ImGuiInputTextFlags_ReadOnly);
	ImGui::EndDisabled();

	ImGui::Text("Vertex Preview");
	VertexRender();
	Vec3 VertexPos = m_MeshCam->Transform()->GetRelativePos();
	float Pos[3] = { VertexPos.x, VertexPos.y, VertexPos.z };
	ImGui::Text("View Pos: ");
	ImGui::SameLine();
	if (ImGui::DragFloat3("##Pos", Pos, 0.01f))
	{
		m_MeshCam->Transform()->SetRelativePos(Pos[0], Pos[1], Pos[2]);
	}
	ImGui::SameLine();
	if (ImGui::Button("Init"))
	{
		m_VertexObject->Transform()->SetRelativePos(Vec3(0, 0, 0));
		m_VertexObject->Transform()->SetRelativeRot(Vec4(0, 0, 0, 1));
	}
}

void CMeshUI::VertexRender()
{
	// 렌더타겟 및 뷰포트 교체
	ComPtr<ID3D11RenderTargetView> ImGuiRTV = nullptr;
	ComPtr<ID3D11DepthStencilView> ImGuiDSV = nullptr;
	D3D11_VIEWPORT ImGuiVP = {};
	UINT VPCount = 0;
	CONTEXT->OMGetRenderTargets(1, ImGuiRTV.GetAddressOf(), ImGuiDSV.GetAddressOf());
	CONTEXT->RSGetViewports(&VPCount, &ImGuiVP);
	CONTEXT->OMSetRenderTargets(1, m_VertexRTTex->GetRTV().GetAddressOf(), m_VertexDSTex->GetDSV().Get());
	CONTEXT->RSSetViewports(1, &m_ViewPort);

	// 렌더 타겟 클리어
	float Color[4] = { 0.f, 0.f, 0.f, 1.f };
	CONTEXT->ClearRenderTargetView(m_VertexRTTex->GetRTV().Get(), Color);
	CONTEXT->ClearDepthStencilView(m_VertexDSTex->GetDSV().Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);

	// 렌더 오브젝트 메쉬 설정, 카메라 틱 및 렌더링 수행
	m_VertexObject->MeshRender()->SetMesh(static_cast<CMesh*>(m_TargetAsset.Get()));
	m_VertexObject->MeshRender()->GetMaterial()->SetScalarParam(VEC4_0, Vec4(0, 1, 0, 1));
	m_VertexObject->MeshRender()->GetMaterial()->GetShader()->SetBSType(BS_TYPE::DEFAULT);
	m_MeshCam->FinalTick();
	m_VertexObject->FinalTick();
	vector<CGameObject*> vec = { m_VertexObject };
	m_MeshCam->Camera()->Direct_Render(vec);

	// 렌더타겟과 뷰포트 원상복구
	CONTEXT->OMSetRenderTargets(1, ImGuiRTV.GetAddressOf(), ImGuiDSV.Get());
	CONTEXT->RSSetViewports(VPCount, &ImGuiVP);

	// ImGui 이미지 UI에 SRV 전달해서 렌더링
	ImGui::Image((ImTextureID)m_VertexRTTex->GetSRV().Get(), ImVec2(300, 300), ImVec2(0, 0), ImVec2(1, 1), ImVec4(1, 1, 1, 1), ImVec4(1, 1, 1, 1));

	// 마우스 조작
	if (ImGui::IsItemHovered() && ImGui::IsMouseDragging(ImGuiMouseButton_Left))
	{
		Vec2 vDrag = CKeyMgr::GetInst()->GetDragDir();

		//Vec3 vObjDir = m_VertexObject->Transform()->GetRelativeRotEuler();
		//vObjDir.y += vDrag.x * EngineDT * 1000.f;
		//vObjDir.x += vDrag.y * EngineDT * 1000.f;
		//m_VertexObject->Transform()->SetRelativeRot(vObjDir);

		Vec4 vQuat = m_VertexObject->Transform()->GetRelativeRotQuat();
		Vec4 qX = XMQuaternionRotationAxis(m_VertexObject->Transform()->GetRelativeDir(DIR::RIGHT), XMConvertToRadians(vDrag.x * EngineDT * 1000.f));
		Vec4 qY = XMQuaternionRotationAxis(Vec3(0, 1, 0), XMConvertToRadians(vDrag.y * EngineDT * 1000.f));
		Vec4 vRotQuat = XMQuaternionMultiply(vQuat, XMQuaternionMultiply(qX, qY));
		m_VertexObject->Transform()->SetRelativeRot(vRotQuat);

		ImGui::Text("Dragging on the button!");
	}
}
