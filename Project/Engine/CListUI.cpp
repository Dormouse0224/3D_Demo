#include "pch.h"
#include "CListUI.h"

CListUI::CListUI(wstring _Name)
	: CImguiObject(_Name)
	, m_vecList{}
	, m_Idx(-1)
	, m_Selected(-1)
{
}

CListUI::~CListUI()
{
}

void CListUI::Update()
{
}

void CListUI::Render()
{
	for (size_t i = 0; i < m_vecList.size(); ++i)
	{
		int Flags = ImGuiTreeNodeFlags_Bullet;

		if (i == m_Idx)
			Flags |= ImGuiTreeNodeFlags_Selected;

		if (ImGui::TreeNodeEx(to_str(m_vecList[i]).c_str(), Flags))
		{
			ImGui::TreePop();
		}

		if (ImGui::IsItemClicked(ImGuiMouseButton_Left))
		{
			m_Idx = i;
		}

		if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
		{
			m_Idx = i;
			m_Selected = m_Idx;
		}
	}
}

void CListUI::Init()
{
	m_Idx = -1;
	m_Selected = -1;
}