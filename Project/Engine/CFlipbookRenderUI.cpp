#include "pch.h"
#include "CFlipbookRenderUI.h"
#include "CFlipbookRender.h"

#include "CAssetMgr.h"

CFlipbookRenderUI::CFlipbookRenderUI()
	: CRenderComponentUI(COMPONENT_TYPE::FLIPBOOKRENDER)
{
}

CFlipbookRenderUI::~CFlipbookRenderUI()
{
}

void CFlipbookRenderUI::Update_Com()
{
}

void CFlipbookRenderUI::Render_Com()
{
    Render_RCom();

    CFlipbookRender* pFlipRender = static_cast<CFlipbookRender*>(m_TargetObj->GetRenderComponent());

    vector<AssetPtr<CFlipbook>> vecFlipbook = pFlipRender->m_vecFlipbook;


    ImGui::SeparatorText("Flipbook Render Option");
    if (ImGui::BeginTable("FlipbookList", 1, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg))
    {
        ImGui::TableSetupColumn("Flipbook List");
        ImGui::TableHeadersRow();
        
        for (int row = 0; row < vecFlipbook.size(); ++row)
        {
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            if (vecFlipbook[row].Get())
                ImGui::TextUnformatted(to_str(vecFlipbook[row]->GetName()).c_str());
            else
                ImGui::Text("N/A");
        }
        ImGui::EndTable();
    }

    // 재생 정보
    AssetPtr<CFlipbook> CurFlipbook = pFlipRender->m_CurFlipbook;
    int& SceneIdx = pFlipRender->m_SceneIdx;
    float& FPS = pFlipRender->m_FPS;
    bool& Repeat = pFlipRender->m_Repeat;

    if (CurFlipbook.Get())
        ImGui::SliderInt("##FlipbookSlider", &SceneIdx, 0, CurFlipbook->GetSceneCount() - 1);
    else
        ImGui::Text("N/A");

    ImGui::Text("FPS");
    ImGui::SameLine();
    ImGui::InputFloat("##FPS", &FPS);

    ImGui::Text("Repeat?");
    ImGui::SameLine();
    ImGui::Checkbox("##Repeat", &Repeat);

    if (ImGui::Button("Add Flipbook"))
    {
        AssetPtr<CFlipbook> pFlip = CAssetMgr::GetInst()->LoadFromFile<CFlipbook>(L".flip");
        if (pFlip.Get())
            pFlipRender->AddFlipbook(pFlip);
    }

    ImGui::SameLine();
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.2f, 0.2f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.8f, 0.4f, 0.4f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.8f, 0.3f, 0.3f, 1.0f));
    if (ImGui::Button("Delete Flipbook"))
    {
        ImGui::OpenPopup("DeleteFlip");
    }
    ImGui::PopStyleColor(3);
    if (ImGui::BeginPopupModal("DeleteFlip", NULL, ImGuiWindowFlags_AlwaysAutoResize))
    {
        float tab = 130.f;
        ImGui::Text("Delete flipbook from component");
        ImGui::NewLine();

        // 레이어 입력
        ImGui::Text("Flipbook select");
        static int FlipIdx = 0;
        vector<const char*> vecChar;
        vector<string> vecStr;
        for (int i = 0; i < vecFlipbook.size(); ++i)
        {
            vecStr.push_back(to_str(vecFlipbook[i]->GetName()));
            vecChar.push_back(vecStr[i].c_str());
        }
        ImGui::Combo("##Flipbook", &FlipIdx, vecChar.data(), vecFlipbook.size());

        // 저장/취소 버튼
        if (ImGui::Button("Delete"))
        {
            pFlipRender->DeleteFlipbook(FlipIdx);
            FlipIdx = 0;
            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();
        if (ImGui::Button("Cancel"))
        {
            FlipIdx = 0;
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }

    if (ImGui::Button("Play Flipbook"))
    {
        ImGui::OpenPopup("PlayFlip");
    }
    if (ImGui::BeginPopupModal("PlayFlip", NULL, ImGuiWindowFlags_AlwaysAutoResize))
    {
        float tab = 130.f;
        ImGui::Text("Setting current flipbook and play");
        ImGui::NewLine();

        // 레이어 입력
        ImGui::Text("Flipbook select");
        static int FlipIdx = 0;
        vector<const char*> vecChar;
        vector<string> vecStr;
        for (int i = 0; i < vecFlipbook.size(); ++i)
        {
            vecStr.push_back(to_str(vecFlipbook[i]->GetName()));
            vecChar.push_back(vecStr[i].c_str());
        }
        ImGui::Combo("##Flipbook", &FlipIdx, vecChar.data(), vecFlipbook.size());

        // 저장/취소 버튼
        if (ImGui::Button("Play"))
        {
            pFlipRender->Play(FlipIdx);
            FlipIdx = 0;
            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();
        if (ImGui::Button("Cancel"))
        {
            FlipIdx = 0;
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }
}
