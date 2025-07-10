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
    vector<tMtrlSet> vecMtrls = m_TargetObj->GetRenderComponent()->m_vecMtrls;
    bool bFrustumCull = m_TargetObj->GetRenderComponent()->GetFrustumCull();

    ImGui::SeparatorText("Basic Render Option");
    ImGui::Text("Frustum Culling");
    if (ImGui::Checkbox("#FrustumCulling", &bFrustumCull))
    {
        m_TargetObj->GetRenderComponent()->SetFrustumCull(bFrustumCull);
    }


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

    for (int i = 0; i < vecMtrls.size() + 1; ++i)
    {
        tMtrlSet MtrlSet = {};
        if (vecMtrls.size() > i)
            MtrlSet = vecMtrls[i];

        ImGui::NewLine();
        string str = "Material #" + to_string(i);
        ImGui::SeparatorText(str.c_str());
        ImGui::Text("Shared Material");
        std::string DefaultMtrlName = "NO EXIST";
        if (MtrlSet.pSharedMtrl.Get())
        {
            DefaultMtrlName = to_str(MtrlSet.pSharedMtrl->GetName());
        }
        string label = "##SharedMtrl" + to_string(i);
        ImGui::InputText(label.c_str(), const_cast<char*>(DefaultMtrlName.c_str())
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

        if (MtrlSet.bUsingDynamic)
        {
            ImGui::Text("Dynamic Material");
            std::string DynamicMtrlName = "NO EXIST";
            if (MtrlSet.pDynamicMtrl.Get())
            {
                DynamicMtrlName = to_str(MtrlSet.pDynamicMtrl->GetName());
            }
            string label = "##DynamicMtrl" + to_string(i);
            ImGui::InputText(label.c_str(), const_cast<char*>(DynamicMtrlName.c_str())
                , DynamicMtrlName.size() + 1, ImGuiInputTextFlags_ReadOnly);
        }
        else
        {
            ImGui::Text("Not Using Dynamic Material..");
        }
    }
}
