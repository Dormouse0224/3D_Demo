#include "pch.h"
#include "CSpriteRenderUI.h"
#include "CSpriteRender.h"

CSpriteRenderUI::CSpriteRenderUI()
	: CRenderComponentUI(COMPONENT_TYPE::SPRITERENDER)
{
}

CSpriteRenderUI::~CSpriteRenderUI()
{
}

void CSpriteRenderUI::Update_Com()
{
}

void CSpriteRenderUI::Render_Com()
{
	Render_RCom();

	AssetPtr<CSprite> Sprite = static_cast<CSpriteRender*>(m_TargetObj->GetRenderComponent())->GetSprite();

    ImGui::SeparatorText("Sprite Render Option");
    ImGui::Text("Sprite");
    std::string SpriteName = "NO EXIST";
    if (Sprite.Get())
    {
        SpriteName = to_str(Sprite->GetName());
    }
    ImGui::InputText("##Sprite", const_cast<char*>(SpriteName.c_str())
        , SpriteName.size() + 1, ImGuiInputTextFlags_ReadOnly);
}
