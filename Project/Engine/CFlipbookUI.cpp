#include "pch.h"
#include "CFlipbookUI.h"

#include "CFlipbook.h"
#include "CTexture2D.h"
#include "CSprite.h"
#include "CAssetMgr.h"

CFlipbookUI::CFlipbookUI()
	: CAssetUI(ASSET_TYPE_WSTR[(UINT)ASSET_TYPE::FLIPBOOK])
{
}

CFlipbookUI::~CFlipbookUI()
{
}

void CFlipbookUI::Update_Ast()
{
}

void CFlipbookUI::Render_Ast()
{
    AssetPtr<CFlipbook> pFlipbook = static_cast<CFlipbook*>(m_TargetAsset.Get());
    float tab = 130;
    static int sprite_current = 0;
    static int texture_current = 0;
    if (sprite_current < 0)
        sprite_current = 0;
    if (texture_current < 0)
        texture_current = 0;

    if (pFlipbook->GetSceneType() == SceneType::SPRITE)
    {
        ImGui::Text("Flipbook Type - Sprite");
        sprite_current = min(sprite_current, pFlipbook->GetSceneCount() - 1);
        if (sprite_current == -1)
            ImGui::Text("Flipbook Empty");
        else
            ImGui::SliderInt("##FlipbookSlider", &sprite_current, 0, pFlipbook->GetSceneCount() - 1);
        AssetPtr<CSprite> pScene = pFlipbook->GetScene<CSprite>(sprite_current);
        if (pScene.Get())
        {
            ImVec2 uv0(pScene->GetLeftTop().x, pScene->GetLeftTop().y);
            ImVec2 uv1(pScene->GetSlice().x + uv0.x, pScene->GetSlice().y + uv0.y);
            ImGui::Image(reinterpret_cast<ImTextureID>(pScene->GetAtlasTex()->GetSRV().Get()), ImVec2(300, 300), uv0, uv1);
        }
        else
            ImGui::Text("Filpbook Empty");
    }
    else if (pFlipbook->GetSceneType() == SceneType::TEXTURE)
    {
        ImGui::Text("Flipbook Type - Texture");
        texture_current = min(texture_current, pFlipbook->GetSceneCount() - 1);
        if (texture_current == -1)
            ImGui::Text("Flipbook Empty");
        else
            ImGui::SliderInt("##FlipbookSlider", &texture_current, 0, pFlipbook->GetSceneCount() - 1);
        AssetPtr<CTexture2D> pScene = pFlipbook->GetScene<CTexture2D>(texture_current);
        if (pScene.Get())
            ImGui::Image(reinterpret_cast<ImTextureID>(pScene->GetSRV().Get()), ImVec2(300, 300));
        else
            ImGui::Text("Filpbook Empty");
    }
    
    if (ImGui::Button("Add Scene"))
    {
        if (pFlipbook->GetSceneType() == SceneType::SPRITE)
        {
            AssetPtr<CSprite> pSprite = CAssetMgr::GetInst()->LoadFromFile<CSprite>(L".sprite");
            if (pSprite.Get())
            {
                pFlipbook->AddScene<CSprite>(pSprite, ++sprite_current);
            }
        }
        else if (pFlipbook->GetSceneType() == SceneType::TEXTURE)
        {
            AssetPtr<CTexture2D> pTex = CAssetMgr::GetInst()->LoadFromFile<CTexture2D>(L"TEX");
            if (pTex.Get())
            {
                pFlipbook->AddScene<CTexture2D>(pTex, ++texture_current);
            }
        }
    }

    ImGui::SameLine();
    if (ImGui::Button("Copy Scene"))
    {
        if (pFlipbook->GetSceneType() == SceneType::SPRITE)
        {
            pFlipbook->AddScene<CSprite>(pFlipbook->GetScene<CSprite>(sprite_current), sprite_current + 1);
        }
        else if (pFlipbook->GetSceneType() == SceneType::TEXTURE)
        {
            pFlipbook->AddScene<CTexture2D>(pFlipbook->GetScene<CTexture2D>(texture_current), texture_current + 1);
        }
    }

    ImGui::SameLine();
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.2f, 0.2f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.8f, 0.4f, 0.4f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.8f, 0.3f, 0.3f, 1.0f));
    if (ImGui::Button("Delete Scene"))
    {
        if (pFlipbook->GetSceneType() == SceneType::SPRITE)
        {
            pFlipbook->DeleteScene(sprite_current);
        }
        else if (pFlipbook->GetSceneType() == SceneType::TEXTURE)
        {
            pFlipbook->DeleteScene(texture_current);
        }
    }
    ImGui::PopStyleColor(3);

    if (ImGui::Button("Change Scehe Type"))
    {
        if (pFlipbook->GetSceneType() == SceneType::SPRITE)
        {
            pFlipbook->SetSceneType(SceneType::TEXTURE);
        }
        else if (pFlipbook->GetSceneType() == SceneType::TEXTURE)
        {
            pFlipbook->SetSceneType(SceneType::SPRITE);
        }
    }
}
