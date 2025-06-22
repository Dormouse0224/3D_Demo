#include "pch.h"
#include "CAssetUI.h"

CAssetUI::CAssetUI(wstring _Name)
	: CImguiObject(_Name)
{
}

CAssetUI::~CAssetUI()
{
}

void CAssetUI::Update()
{
	Update_Ast();
}

void CAssetUI::Render()
{
	Render_Ast();
}
