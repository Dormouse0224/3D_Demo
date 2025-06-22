#include "pch.h"
#include "CImguiObject.h"

CImguiObject::CImguiObject(wstring _Name)
	: CEntity(_Name)
	, m_Parent(nullptr)
	, m_vecChild{}
	, m_Active(false)
	, m_Modal(false)
	, m_Seperator(false)
	, m_ChildSize(0.f, 0.f)
{
	m_Key = string(_Name.begin(), _Name.end()) + "##" + to_string(GetID());
}

CImguiObject::~CImguiObject()
{
	for (auto& child : m_vecChild)
	{
		delete child;
	}
	m_vecChild.clear();
}

void CImguiObject::Update_Progress()
{
	Update();

	for (const auto& child : m_vecChild)
	{
		child->Update();
	}
}

void CImguiObject::Render_Progress()
{
	if (!m_Active)
		return;

	// 최상위 부모 UI 인 경우
	if (nullptr == m_Parent)
	{
		// Modalless
		if (!m_Modal)
		{
			ImGui::Begin(m_Key.c_str(), &m_Active);

			Render();

			for (const auto& child : m_vecChild)
			{
				child->Render();
			}

			ImGui::End();
		}

		// Modal
		else
		{
			ImGui::OpenPopup(m_Key.c_str());
			if (ImGui::BeginPopupModal(m_Key.c_str(), &m_Active, ImGuiWindowFlags_AlwaysAutoResize))
			{
				Render();

				for (const auto& child : m_vecChild)
				{
					child->Render();
				}

				ImGui::EndPopup();
			}
		}
	}

	// 자식 UI  인 경우
	else
	{
		ImGui::BeginChild(m_Key.c_str(), m_ChildSize);


		Render();

		for (const auto& child : m_vecChild)
		{
			child->Render();
		}

		ImGui::EndChild();

		// 구분선
		if (m_Seperator)
			ImGui::Separator();
	}
}


