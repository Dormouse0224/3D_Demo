#include "pch.h"
#include "CMeshRenderUI.h"

CMeshRenderUI::CMeshRenderUI()
	: CRenderComponentUI(COMPONENT_TYPE::MESHRENDER)
{
}

CMeshRenderUI::~CMeshRenderUI()
{
}

void CMeshRenderUI::Update_Com()
{
}

void CMeshRenderUI::Render_Com()
{
	Render_RCom();
}
