#include "pch.h"
#include "CTransformUI.h"

#include "CGameObject.h"
#include "CTransform.h"

CTransformUI::CTransformUI()
    : CComponentUI(COMPONENT_TYPE::TRANSFORM)
{
}

CTransformUI::~CTransformUI()
{
}

void CTransformUI::Update_Com()
{
}

void CTransformUI::Render_Com()
{
    CTransform* pTransform = dynamic_cast<CTransform*>(m_TargetObj->GetComponent(m_Type));

    if (pTransform == nullptr)
        return;

    float tab = 160.f;

    ImGui::Text("Position");
    ImGui::SameLine(tab);
    float Position[3] = { pTransform->GetRelativePos().x,  pTransform->GetRelativePos().y,  pTransform->GetRelativePos().z };
    if (ImGui::DragFloat3("##Position", Position))
    {
        pTransform->SetRelativePos(Vec3(Position));
    }
    ImGui::Spacing();

    ImGui::Text("Scale");
    ImGui::SameLine(tab);
    float Scale[3] = { pTransform->GetRelativeScale().x,  pTransform->GetRelativeScale().y,  pTransform->GetRelativeScale().z };
    if (ImGui::DragFloat3("##Scale", Scale))
    {
        pTransform->SetRelativeScale(Vec3(Scale));
    }
    ImGui::Spacing();

    ImGui::Text("Rotation");
    ImGui::SameLine(tab);
    float Rotation[3] = { pTransform->GetRelativeRotEuler().x,  pTransform->GetRelativeRotEuler().y,  pTransform->GetRelativeRotEuler().z };
    if (ImGui::DragFloat3("##Rotation", Rotation))
    {
        pTransform->SetRelativeRot(Vec3(Rotation));
    }
    ImGui::Spacing();

    ImGui::Text("Independent Scale");
    ImGui::SameLine(tab);
    bool bIS = pTransform->GetIndependentScale();
    if (ImGui::Checkbox("##IndependentScale", &bIS))
    {
        pTransform->SetIndependentScale(bIS);
    }
    ImGui::Spacing();

    ImGui::Text("Independent Rotation");
    ImGui::SameLine(tab);
    bool bIR = pTransform->GetIndependentRot();
    if (ImGui::Checkbox("##IndependentRotation", &bIR))
    {
        pTransform->SetIndependentRot(bIR);
    }
    ImGui::Spacing();

    ImGui::Text("Independent Position");
    ImGui::SameLine(tab);
    bool bIP = pTransform->GetIndependentTrans();
    if (ImGui::Checkbox("##IndependentPosition", &bIP))
    {
        pTransform->SetIndependentTrans(bIP);
    }
    ImGui::Spacing();
}
