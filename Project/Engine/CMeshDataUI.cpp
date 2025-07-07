#include "pch.h"

#include "CMeshDataUI.h"
#include "CMeshData.h"
#include "CGameObject.h"

#include "CLevelMgr.h"

CMeshDataUI::CMeshDataUI()
    : CAssetUI(ASSET_TYPE_WSTR[(UINT)ASSET_TYPE::MESHDATA])
{
}

CMeshDataUI::~CMeshDataUI()
{
}

void CMeshDataUI::Update_Ast()
{
}

void CMeshDataUI::Render_Ast()
{
    if (ImGui::Button("Instanciate"))
    {
        ImGui::OpenPopup("MeshDataInstanciate");
    }
    if (ImGui::BeginPopupModal("MeshDataInstanciate", NULL, ImGuiWindowFlags_AlwaysAutoResize))
    {
        float tab = 130.f;
        ImGui::Text("Instanciate MeshData to GameObject.");
        ImGui::NewLine();

        // 오브젝트 이름 입력
        ImGui::Text("Name: ");
        ImGui::SameLine(tab);
        static char Name[255] = {};
        ImGui::InputText("##Name", Name, 255);

        // 레이어 입력
        ImGui::Text("Layer select");
        static int layeridx = 0;
        string layerstr[static_cast<int>(LAYER::END)] = {};
        const char* layer[static_cast<int>(LAYER::END)] = {};
        for (int i = 0; i < static_cast<int>(LAYER::END); ++i)
        {
            layerstr[i] = to_str(LAYER_WSTR[i]);
            layer[i] = layerstr[i].c_str();
        }
        ImGui::Combo("##Layer", &layeridx, layer, static_cast<int>(LAYER::END));

        if (ImGui::Button("Instanciate"))
        {
            if (CLevelMgr::GetInst()->GetCurrentLevel())
            {
                CMeshData* pMeshData = reinterpret_cast<CMeshData*>(m_TargetAsset.Get());
                CGameObject* pGameObject = pMeshData->Instantiate();
                pGameObject->SetName(to_wstr(Name));
                CLevelMgr::GetInst()->GetCurrentLevel()->AddGameObject(pGameObject, layeridx, true);
            }
            layeridx = 0;
            memset(Name, 0, sizeof(Name));
            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();
        if (ImGui::Button("Cancel"))
        {
            layeridx = 0;
            memset(Name, 0, sizeof(Name));
            ImGui::CloseCurrentPopup();
        }


        ImGui::EndPopup();
    }
}
