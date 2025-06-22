#include "pch.h"
#include "CComponentUI.h"

CComponentUI::CComponentUI(COMPONENT_TYPE _Type)
	: CImguiObject(COMPONENT_TYPE_WSTR[(UINT)_Type])
	, m_Type(_Type)
{
}

CComponentUI::~CComponentUI()
{
}

void CComponentUI::Update()
{
	Update_Com();
}

void CComponentUI::Render()
{
	if (ImGui::CollapsingHeader(to_str(COMPONENT_TYPE_WSTR[(UINT)m_Type]).c_str()))
	{
		Render_Com();
		ImGui::Separator();
	}

}
