#include "pch.h"
#include "CAnimator3DUI.h"
#include "CAnimator3D.h"

CAnimator3DUI::CAnimator3DUI()
    : CComponentUI(COMPONENT_TYPE::ANIMATOR3D)
{
}

CAnimator3DUI::~CAnimator3DUI()
{
}

void CAnimator3DUI::Update_Com()
{
}

void CAnimator3DUI::Render_Com()
{
    CAnimator3D* pAnim = m_TargetObj->Animator3D();
    if (pAnim == nullptr)
        return;

    ImGui::Text("Current Animation");
    int curClipIdx = pAnim->GetCurClipIdx();
    string str = to_str(pAnim->GetClipVec()->at(curClipIdx).strAnimName);
    ImGui::Text(str.c_str());
    if (ImGui::SliderInt("##CurAnim", &curClipIdx, 0, pAnim->GetClipVec()->size() - 1))
    {
        pAnim->SetCurClipIdx(curClipIdx);
    }

    ImGui::NewLine();
    ImGui::Text("Leg IK : Left");
    float LeftIK[3] = { pAnim->GetLeftLegIK().x, pAnim->GetLeftLegIK().y, pAnim->GetLeftLegIK().z };
    if (ImGui::DragFloat3("##IK_L", LeftIK, 0.01f))
    {
        pAnim->SetLeftLegIK(Vec3(LeftIK));
    }

    ImGui::NewLine();
    ImGui::Text("Leg IK : Right");
    float RightIK[3] = { pAnim->GetRightLegIK().x, pAnim->GetRightLegIK().y, pAnim->GetRightLegIK().z };
    if (ImGui::DragFloat3("##IK_R", RightIK, 0.01f))
    {
        pAnim->SetRightLegIK(Vec3(RightIK));
    }
}
