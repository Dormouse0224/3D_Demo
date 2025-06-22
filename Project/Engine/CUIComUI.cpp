#include "pch.h"
#include "CUIComUI.h"

#include "CAssetMgr.h"

#include "CGameObject.h"
#include "CUICom.h"
#include "CFont.h"

CUIComUI::CUIComUI()
	: CRenderComponentUI(COMPONENT_TYPE::UICOM)
{
}

CUIComUI::~CUIComUI()
{
}

void CUIComUI::Update_Com()
{
}

void CUIComUI::Render_Com()
{
	Render_RCom();
	
	// UI 크기 및 위치 설정
	ImGui::Text("UI Position");
	float pos[2] = { m_TargetObj->UICom()->m_ScreenPosNDC.x, m_TargetObj->UICom()->m_ScreenPosNDC.y };
	if (ImGui::DragFloat2("##UIPosition", pos, 0.001f))
	{
		m_TargetObj->UICom()->m_ScreenPosNDC = Vec2(pos);
	}

	ImGui::NewLine();
	ImGui::Text("UI Scale");
	float scale[2] = { m_TargetObj->UICom()->m_Scale.x, m_TargetObj->UICom()->m_Scale.y };
	if (ImGui::DragFloat2("##UIScale", scale))
	{
		m_TargetObj->UICom()->m_Scale = Vec2(scale);
	}

	ImGui::NewLine();
	ImGui::Text("UI Alpha");
	float alpha = m_TargetObj->UICom()->m_Alpha;
	if (ImGui::DragFloat("##UIAlpha", &alpha, 0.01f))
	{
		m_TargetObj->UICom()->m_Alpha = alpha;
	}

	ImGui::NewLine();
    ImGui::Text("UI Text List");
    if (ImGui::BeginTable("##UITextList", 3, ImGuiTableFlags_NoSavedSettings | ImGuiTableFlags_Borders))
	{
		ImGui::TableSetupColumn("Font");
		ImGui::TableSetupColumn("Text");
		ImGui::TableHeadersRow();
		auto& vecUIText = m_TargetObj->UICom()->m_vecUIText;

		for (int i = 0; i < vecUIText.size(); ++i)
		{
			const auto& t = vecUIText[i];
			AssetPtr<CFont> pFont = get<0>(t);
			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			ImGui::Text(to_str(pFont->GetName()).c_str());
			ImGui::TableSetColumnIndex(1);
			ImGui::Text(to_str(get<1>(t)).c_str());
			ImGui::TableSetColumnIndex(2);
			string str = "Delete##" + to_string(i);
			if (ImGui::Button(str.c_str()))
			{
				delete get<2>(t);
				vecUIText.erase(vecUIText.begin() + i);
			}
		}
		ImGui::EndTable();
    }

	ImGui::NewLine();
	if (ImGui::Button("Add UI Text"))
	{
		ImGui::OpenPopup("AddUIText");
	}
	if (ImGui::BeginPopupModal("AddUIText", NULL, ImGuiWindowFlags_AlwaysAutoResize))
	{
		float tab = 130.f;
		ImGui::Text("Add condition to FSM.");
		ImGui::NewLine();

		// Font Type
		ImGui::Text("Font Type");
		static int FontIdx = 0;
		vector<const char*> vec;
		auto vecstr = CAssetMgr::GetInst()->GetAssetList<CFont>();
		for (int i = 0; i < vecstr.size(); ++i)
			vec.push_back(vecstr[i].c_str());
		ImGui::Combo("##Font", &FontIdx, vec.data(), vec.size());

		// Text
		ImGui::Text("Text");
		static char buff[255] = {};
		ImGui::InputText("##Text", buff, 255);

		// Color
		ImGui::Text("Color");
		static float color[4];
		ImGui::ColorPicker4("##Color", color);

		// Rotation
		ImGui::Text("Rotation");
		static float rot = 0;
		ImGui::InputFloat("##Rot", &rot);

		// Scale
		ImGui::Text("Scale");
		static float scale = 1;
		ImGui::InputFloat("##Scale", &scale);


		if (ImGui::Button("Add"))
		{
			tUITextDesc* pDesc = new tUITextDesc;
			pDesc->Color = Vec4(color);
			pDesc->Rot = rot;
			pDesc->Scale = scale;

			m_TargetObj->UICom()->AddUIText(CAssetMgr::GetInst()->Load<CFont>(to_wstr(vec[FontIdx])), to_wstr(buff), pDesc);
			FontIdx = 0; memset(buff, 0, sizeof(buff)); memset(color, 0, sizeof(color)); scale = 1;
			ImGui::CloseCurrentPopup();
		}
		ImGui::SameLine();
		if (ImGui::Button("Cancel"))
		{
			FontIdx = 0; memset(buff, 0, sizeof(buff)); memset(color, 0, sizeof(color)); rot = 0; scale = 1;
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}
}
