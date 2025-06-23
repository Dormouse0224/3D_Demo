#include "pch.h"
#include "CTexture2DUI.h"
#include "CTexture.h"

CTexture2DUI::CTexture2DUI()
	: CAssetUI(ASSET_TYPE_WSTR[(UINT)ASSET_TYPE::TEXTURE])
{
}

CTexture2DUI::~CTexture2DUI()
{
}

void CTexture2DUI::Update_Ast()
{
}

void CTexture2DUI::Render_Ast()
{
    AssetPtr<CTexture> pTex = static_cast<CTexture*>(m_TargetAsset.Get());
    ImGui::Image((ImTextureID)pTex->GetSRV().Get(), ImVec2(300, 300), ImVec2(0, 0), ImVec2(1, 1), ImVec4(1, 1, 1, 1), ImVec4(1, 1, 1, 1));
}
