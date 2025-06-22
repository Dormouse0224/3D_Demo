#include "pch.h"
#include "CSpriteUI.h"

#include "CSprite.h"
#include "CTexture2D.h"

CSpriteUI::CSpriteUI()
	: CAssetUI(ASSET_TYPE_WSTR[(UINT)ASSET_TYPE::SPRITE])
{
}

CSpriteUI::~CSpriteUI()
{
}

void CSpriteUI::Update_Ast()
{
}

void CSpriteUI::Render_Ast()
{
    AssetPtr<CSprite> pSprite = static_cast<CSprite*>(m_TargetAsset.Get());
    AssetPtr<CTexture2D> pAtlas = pSprite->GetAtlasTex();
    float tab = 130;

    // 스프라이트의 아틀라스 텍스쳐 정보
    ImGui::Text("Atlas Texture");
    string Name = "N/A";
    if (pAtlas.Get())
        Name = to_str(pAtlas->GetName());
    ImGui::InputText("##AtlasTex", const_cast<char*>(Name.c_str()), Name.size() + 1, ImGuiInputTextFlags_ReadOnly);
    if (ImGui::BeginDragDropTarget())
    {
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ContentViewer"))
        {
            CAsset* Asset = *reinterpret_cast<CAsset**>(payload->Data);
            AssetPtr<CTexture2D> pTex = dynamic_cast<CTexture2D*>(Asset);
            if (pTex.Get())
            {
                pSprite->SetAtlasTex(pTex);
            }
        }
        ImGui::EndDragDropTarget();
    }

    // 스프라이트 이미지
    ImGui::NewLine();
    ImGui::Text("Sprite");
    if (pAtlas.Get())
    {
        ImVec2 uv0(pSprite->GetLeftTop().x, pSprite->GetLeftTop().y);
        ImVec2 uv1(pSprite->GetSlice().x + uv0.x, pSprite->GetSlice().y + uv0.y);
        ImGui::Image(reinterpret_cast<ImTextureID>(pAtlas->GetSRV().Get()), ImVec2(300, 300), uv0, uv1, ImVec4(1, 1, 1, 1), ImVec4(1, 1, 1, 1));
    }
    else
        ImGui::Text("N/A");

    if (pAtlas.Get())
    {
        // LeftTop
        ImGui::NewLine();
        ImGui::Text("LeftTop");
        ImGui::Text("X : ");
        ImGui::SameLine();
        float LeftTopX = pSprite->GetLeftTop().x * pAtlas->GetWidth();
        if (ImGui::InputFloat("##LeftTopX", &LeftTopX, pSprite->GetSlice().x * pAtlas->GetWidth()))
        {
            pSprite->SetLeftTop(Vec2(LeftTopX, pSprite->GetLeftTop().y * pAtlas->GetHeight()));
        }
        ImGui::Text("Y : ");
        ImGui::SameLine();
        float LeftTopY = pSprite->GetLeftTop().y * pAtlas->GetHeight();
        if (ImGui::InputFloat("##LeftTopY", &LeftTopY, pSprite->GetSlice().y * pAtlas->GetHeight()))
        {
            pSprite->SetLeftTop(Vec2(pSprite->GetLeftTop().x * pAtlas->GetWidth(), LeftTopY));
        }

        // Slice
        ImGui::NewLine();
        ImGui::Text("Slice");
        float Slice[2] = { pSprite->GetSlice().x * pAtlas->GetWidth(), pSprite->GetSlice().y * pAtlas->GetHeight() };
        if (ImGui::InputFloat2("##Slice", Slice))
        {
            pSprite->SetSlice(Vec2(Slice));
        }
    }
}
