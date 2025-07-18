#include "pch.h"
#include "CLightUI.h"

#include "CGameObject.h"
#include "CLight.h"

CLightUI::CLightUI()
	: CComponentUI(COMPONENT_TYPE::LIGHT)
{
}

CLightUI::~CLightUI()
{
}

void CLightUI::Update_Com()
{
}

void CLightUI::Render_Com()
{
    ImGui::Text("Cascade Shadow Mapping");
    bool bCSM = m_TargetObj->Light()->GetCSM();
    ImGui::SameLine(200.f);
    if (ImGui::Checkbox("##CSM", &bCSM))
    {
        m_TargetObj->Light()->SetCSM(bCSM);
    }

	tLightModule LightModule = m_TargetObj->Light()->GetLightModule();

	// Font Type
	ImGui::Text("Light Type");
	int LightIdx = (int)LightModule.Type;
	const char* ch[3] = { "DIRECTIONAL", "POINT", "SPOT" };
	if (ImGui::Combo("##LightType", &LightIdx, ch, 3))
	{
		LightModule.Type = (LIGHT_TYPE)LightIdx;
		m_TargetObj->Light()->SetLightModule(LightModule);
	}

	// Color
	ImGui::Text("Color");
	float fcol[3] = { LightModule.Color.x, LightModule.Color.y, LightModule.Color.z};
	if (ImGui::InputFloat3("##Color", fcol))
	{
		LightModule.Color = Vec3(fcol[0], fcol[1], fcol[2]);
		m_TargetObj->Light()->SetLightModule(LightModule);
	}

	// WorldPos
	ImGui::Text("WorldPos");
	float fWP[3] = { LightModule.WorldPos.x, LightModule.WorldPos.y, LightModule.WorldPos.z };
    ImGui::BeginDisabled();
    ImGui::InputFloat3("##WorldPos", fWP);
    ImGui::EndDisabled();

	// Radius
	ImGui::Text("Radius");
	float fRad = LightModule.Radius;
	if (ImGui::DragFloat("##Radius", &fRad))
	{
        if (fRad < 2.f)
            fRad = 2.f;
		LightModule.Radius = fRad;
		m_TargetObj->Light()->SetLightModule(LightModule);
	}

	// Dir
	ImGui::Text("Dir");
	float fDir[3] = { LightModule.Dir.x, LightModule.Dir.y, LightModule.Dir.z };
    ImGui::BeginDisabled();
    ImGui::InputFloat3("##Dir", fDir);
    ImGui::EndDisabled();

	// Angle
	ImGui::Text("Angle");
	float fAng = LightModule.Angle;
	if (ImGui::DragFloat("##Angle", &fAng))
	{
        if (fAng < 1.f)
            fAng = 1.f;
		LightModule.Angle = fAng;
		m_TargetObj->Light()->SetLightModule(LightModule);
	}
}
