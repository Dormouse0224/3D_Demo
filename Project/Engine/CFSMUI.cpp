#include "pch.h"
#include "CFSMUI.h"

#include "CFSMMgr.h"

#include "CFSM.h"
#include "CFSM_State.h"

CFSMUI::CFSMUI()
	:CComponentUI(COMPONENT_TYPE::FSM)
{
}

CFSMUI::~CFSMUI()
{
}

void CFSMUI::Update_Com()
{
}

void CFSMUI::Render_Com()
{
    vector<FSM_Condition*>              m_vecCondition;
    CFSM_State* m_CurrentState;
    CFSM_State* m_InitState;

	// 조건 목록 표시
	if (ImGui::BeginTable("##Conditions", 4, ImGuiTableFlags_NoSavedSettings | ImGuiTableFlags_Borders))
	{
		ImGui::TableSetupColumn("Index");
		ImGui::TableSetupColumn("Origin State");
		ImGui::TableSetupColumn("Destination State");
		ImGui::TableSetupColumn("Trigger Function");
		ImGui::TableHeadersRow();

		auto vec = m_TargetObj->FSM()->m_vecCondition;
		for (int i = 0; i < vec.size(); ++i)
		{
			if (vec[i])
			{
				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0);
				ImGui::Text(to_string(i).c_str());
				ImGui::TableSetColumnIndex(1);
				ImGui::Text(typeid(*(vec[i]->m_OriginState)).name());
				ImGui::TableSetColumnIndex(2);
				ImGui::Text(typeid(*(vec[i]->m_DestState)).name());
				ImGui::TableSetColumnIndex(3);
				ImGui::Text(vec[i]->m_FuncName.c_str());
			}
		}
		ImGui::EndTable();
	}
    
	ImGui::NewLine();
	ImGui::Text("Current State : ");
	ImGui::SameLine();
	if (m_TargetObj->FSM()->m_CurrentState)
		ImGui::TextColored(ImVec4(0.2f, 0.2f, 0.6f, 1.f), typeid(*m_TargetObj->FSM()->m_CurrentState).name());
	else
		ImGui::Text("N/A");
	ImGui::Text("Init State : ");
	ImGui::SameLine();
	if (m_TargetObj->FSM()->m_InitState)
		ImGui::TextColored(ImVec4(0.2f, 0.6f, 0.2f, 1.f), typeid(*m_TargetObj->FSM()->m_InitState).name());
	else
		ImGui::Text("N/A");

	if (ImGui::Button("Add Condition"))
	{
		ImGui::OpenPopup("AddCond");
	}
	if (ImGui::BeginPopupModal("AddCond", NULL, ImGuiWindowFlags_AlwaysAutoResize))
	{
		float tab = 130.f;
		ImGui::Text("Add condition to FSM.");
		ImGui::NewLine();

		// Origin State
		ImGui::NewLine();
		ImGui::Text("Origin State");
		static int OSIdx = 0;
		ImGui::Combo("##OriginState", &OSIdx, CFSMMgr::GetInst()->GetStateRegistryList().data(), CFSMMgr::GetInst()->GetStateRegistryList().size());
		
		// Dest State
		ImGui::NewLine();
		ImGui::Text("Dest State");
		static int DSIdx = 0;
		ImGui::Combo("##DestState", &DSIdx, CFSMMgr::GetInst()->GetStateRegistryList().data(), CFSMMgr::GetInst()->GetStateRegistryList().size());

		// Trigger Func
		ImGui::NewLine();
		ImGui::Text("Trigger Func");
		static int TFIdx = 0;
		ImGui::Combo("##TriggerFunc", &TFIdx, CFSMMgr::GetInst()->GetTriggerRegistryList().data(), CFSMMgr::GetInst()->GetTriggerRegistryList().size());

		if (ImGui::Button("Add"))
		{
			m_TargetObj->FSM()->AddCondition(CFSMMgr::GetInst()->GetStateRegistryList()[OSIdx]
											, CFSMMgr::GetInst()->GetStateRegistryList()[DSIdx]
											, CFSMMgr::GetInst()->GetTriggerRegistryList()[TFIdx]);
			OSIdx = DSIdx = TFIdx = 0;
			ImGui::CloseCurrentPopup();
		}
		ImGui::SameLine();
		if (ImGui::Button("Cancel"))
		{
			OSIdx = DSIdx = TFIdx = 0;
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}

	ImGui::SameLine();
	if (ImGui::Button("Delete Condition"))
	{
		if (!m_TargetObj->FSM()->m_vecCondition.empty())
			ImGui::OpenPopup("DelCond");
	}
	if (ImGui::BeginPopupModal("DelCond", NULL, ImGuiWindowFlags_AlwaysAutoResize))
	{
		float tab = 130.f;
		ImGui::Text("Delete condition from FSM.");
		ImGui::NewLine();

		// Idx
		static int Idx = 0;
		ImGui::SliderInt("##Idx", &Idx, 0, m_TargetObj->FSM()->m_vecCondition.size() - 1);

		if (ImGui::Button("Delete"))
		{
			m_TargetObj->FSM()->DeleteCondition(Idx);
			Idx = 0;
			ImGui::CloseCurrentPopup();
		}
		ImGui::SameLine();
		if (ImGui::Button("Cancel"))
		{
			Idx = 0;
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}

	ImGui::SameLine();
	if (ImGui::Button("Set Init State"))
	{
		ImGui::OpenPopup("SetInitState");
	}
	if (ImGui::BeginPopupModal("SetInitState", NULL, ImGuiWindowFlags_AlwaysAutoResize))
	{
		float tab = 130.f;
		ImGui::Text("Set initial state to FSM.");
		ImGui::Text("If state is not exist in condition list, progress will be ignored.");
		ImGui::NewLine();

		// Init State
		static int InitIdx = 0;
		ImGui::Combo("##OriginState", &InitIdx, CFSMMgr::GetInst()->GetStateRegistryList().data(), CFSMMgr::GetInst()->GetStateRegistryList().size());

		if (ImGui::Button("Add"))
		{
			m_TargetObj->FSM()->SetInitState(CFSMMgr::GetInst()->GetStateRegistryList()[InitIdx]);
			InitIdx = 0;
			ImGui::CloseCurrentPopup();
		}
		ImGui::SameLine();
		if (ImGui::Button("Cancel"))
		{
			InitIdx = 0;
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}
}
