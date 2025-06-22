#include "pch.h"
#include "CRenderComponentUI.h"

CRenderComponentUI::CRenderComponentUI(COMPONENT_TYPE _Type)
	: CComponentUI(_Type)
{
}

CRenderComponentUI::~CRenderComponentUI()
{
}

void CRenderComponentUI::Update_RCom()
{
}

void CRenderComponentUI::Render_RCom()
{

    AssetPtr<CMesh> Mesh = m_TargetObj->GetRenderComponent()->m_Mesh;
    AssetPtr<CMaterial> DefaultMtrl = m_TargetObj->GetRenderComponent()->m_DefaultMtrl;
    AssetPtr<CMaterial> DynamicMtrl = m_TargetObj->GetRenderComponent()->m_DynamicMtrl;
    bool UsingDynamic = m_TargetObj->GetRenderComponent()->m_UsingDynamic;

    ImGui::SeparatorText("Basic Render Option");
    ImGui::Text("Mesh");
    std::string MeshName = "NO EXIST";
    if (Mesh.Get())
    {
        MeshName = to_str(Mesh->GetName());
    }
    ImGui::InputText("##Mesh", const_cast<char*>(MeshName.c_str())
        , MeshName.size() + 1, ImGuiInputTextFlags_ReadOnly);
    if (ImGui::BeginDragDropTarget())
    {
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ContentViewer"))
        {
            CAsset* Asset = *reinterpret_cast<CAsset**>(payload->Data);
            AssetPtr<CMesh> pMesh = dynamic_cast<CMesh*>(Asset);
            if (pMesh.Get())
            {
                m_TargetObj->GetRenderComponent()->SetMesh(pMesh);
            }
        }
        ImGui::EndDragDropTarget();
    }

    ImGui::Text("Default Material");
    std::string DefaultMtrlName = "NO EXIST";
    if (DefaultMtrl.Get())
    {
        DefaultMtrlName = to_str(DefaultMtrl->GetName());
    }
    ImGui::InputText("##DefaultMtrl", const_cast<char*>(DefaultMtrlName.c_str())
        , DefaultMtrlName.size() + 1, ImGuiInputTextFlags_ReadOnly);
    if (ImGui::BeginDragDropTarget())
    {
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ContentViewer"))
        {
            CAsset* Asset = *reinterpret_cast<CAsset**>(payload->Data);
            AssetPtr<CMaterial> pMtrl = dynamic_cast<CMaterial*>(Asset);
            if (pMtrl.Get())
            {
                m_TargetObj->GetRenderComponent()->SetMaterial(pMtrl);
            }
        }
        ImGui::EndDragDropTarget();
    }

    if (UsingDynamic)
    {
        ImGui::Text("Dynamic Material");
        std::string DynamicMtrlName = "NO EXIST";
        if (DynamicMtrl.Get())
        {
            DynamicMtrlName = to_str(DynamicMtrl->GetName());
        }
        ImGui::InputText("##DefaultMtrl", const_cast<char*>(DynamicMtrlName.c_str())
            , DynamicMtrlName.size() + 1, ImGuiInputTextFlags_ReadOnly);
    }
    else
    {
        ImGui::Text("Not Using Dynamic Material..");
    }
}
