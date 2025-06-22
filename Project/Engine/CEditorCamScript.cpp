#include "pch.h"
#include "CEditorCamScript.h"

#include "CTimeMgr.h"
#include "CKeyMgr.h"
#include "CLevelMgr.h"
#include "CImguiMgr.h"

#include "CTransform.h"
#include "CCamera.h"
#include "CLevel.h"
#include "CEngine.h"

CEditorCamScript::CEditorCamScript()
	: m_CamSpeedLin(500.f)
	, m_CamSpeedAng(100.f)
{
}

CEditorCamScript::~CEditorCamScript()
{
}

void CEditorCamScript::Tick()
{
	CLevel* pLevel = CLevelMgr::GetInst()->GetCurrentLevel();
	if (pLevel == nullptr || pLevel->GetState() != LEVEL_STATE::STOP)
		return;

	// 카메라 컴포넌트가 없으면 종료
	if (!Camera())
		return;

	PROJ_TYPE type = Camera()->GetProjType();

	// 메인 윈도우가 포커싱 중일 때만 조작
	if (GetForegroundWindow() == CEngine::GetInst()->GetMainWndHwnd())
	{
		if (PROJ_TYPE::PERSPECTIVE == type)
			Move_Perspective();
		else
			Move_OrthoGraphic();
	}
}

int CEditorCamScript::Load(fstream& _Stream)
{
	_Stream.read(reinterpret_cast<char*>(&m_CamSpeedLin), sizeof(float));

	return S_OK;
}

int CEditorCamScript::Save(fstream& _Stream)
{
	_Stream.write(reinterpret_cast<char*>(&m_CamSpeedLin), sizeof(float));

	return S_OK;
}

void CEditorCamScript::Move_Perspective()
{
	Vec3 vPos = Transform()->GetRelativePos();

	Vec3 vFront = Transform()->GetRelativeDir(DIR::FRONT);
	Vec3 vRight = Transform()->GetRelativeDir(DIR::RIGHT);

	if (KEY_PRESSED(Keyboard::W))
		vPos += vFront * EngineDT * m_CamSpeedLin;
	if (KEY_PRESSED(Keyboard::S))
		vPos -= vFront * EngineDT * m_CamSpeedLin;
	if (KEY_PRESSED(Keyboard::A))
		vPos -= vRight * EngineDT * m_CamSpeedLin;
	if (KEY_PRESSED(Keyboard::D))
		vPos += vRight * EngineDT * m_CamSpeedLin;

	Transform()->SetRelativePos(vPos);


	if (!KEY_PRESSED(Keyboard::MOUSE_RBTN))
		return;

	Vec2 vDragDir = CKeyMgr::GetInst()->GetDragDir();

	//Vec3 vRot = Transform()->GetRelativeRotEuler();
	//vRot.x += vDragDir.y * EngineDT * m_CamSpeedAng;
	//vRot.y += vDragDir.x * EngineDT * m_CamSpeedAng;
	//Transform()->SetRelativeRot(vRot);

	Vec4 vQuat = Transform()->GetRelativeRotQuat();
	Vec4 qX = XMQuaternionRotationAxis(Transform()->GetRelativeDir(DIR::RIGHT), XMConvertToRadians(vDragDir.y * EngineDT * m_CamSpeedAng));
	Vec4 qY = XMQuaternionRotationAxis(Vec3(0, 1, 0), XMConvertToRadians(vDragDir.x * EngineDT * m_CamSpeedAng));
	Vec4 vRotQuat = XMQuaternionMultiply(vQuat, XMQuaternionMultiply(qX, qY));
	Transform()->SetRelativeRot(vRotQuat);
}

void CEditorCamScript::Move_OrthoGraphic()
{
	Vec4 vRot = Vec4(0.f, 0.f, 0.f, 1.f);
	Transform()->SetRelativeRot(vRot);

	Vec3 vPos = Transform()->GetRelativePos();

	if (KEY_PRESSED(Keyboard::W))
		vPos.y += EngineDT * m_CamSpeedLin;
	if (KEY_PRESSED(Keyboard::S))
		vPos.y -= EngineDT * m_CamSpeedLin;
	if (KEY_PRESSED(Keyboard::A))
		vPos.x -= EngineDT * m_CamSpeedLin;
	if (KEY_PRESSED(Keyboard::D))
		vPos.x += EngineDT * m_CamSpeedLin;

	Transform()->SetRelativePos(vPos);
}
