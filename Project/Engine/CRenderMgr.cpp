#include "pch.h"
#include "CRenderMgr.h"

#include "CDevice.h"
#include "CConstBuffer.h"
#include "CCamera.h"
#include "CTransform.h"
#include "CLevel.h"
#include "CRenderComponent.h"
#include "CLayer.h"
#include "CMRT.h"

#include "CKeyMgr.h"
#include "CAssetMgr.h"
#include "CTimeMgr.h"
#include "CPhysxMgr.h"
#include "CLevelMgr.h"
#include "CImguiMgr.h"
#include "CLightMgr.h"

CRenderMgr::CRenderMgr()
{

}

CRenderMgr::~CRenderMgr()
{
	if (nullptr != m_DebugObject)
		delete m_DebugObject;

	if (nullptr != m_EditorCam)
		delete m_EditorCam;

	if (nullptr != m_UICam)
		delete m_UICam;

	for (int i = 0; i < MRT_END; ++i)
	{
		if (m_arrMRT[i] != nullptr)
			delete m_arrMRT[i];
	}
}

void CRenderMgr::RegisterCamera(CCamera* _Cam, int _Priority)
{
	assert(_Priority >= 0);	// Priority 는 0 이상이어야 합니다.

	if (m_vecCam.size() <= _Priority)
	{
		m_vecCam.resize(_Priority + 1);
	}

	// 입력받은 priority 자리에 다른 카메라가 이미 있는 경우, 기존 카메라를 priority -1 로 설정 후 자리를 덮어 쓴다.
	if (m_vecCam[_Priority] != nullptr)
	{
		m_vecCam[_Priority]->m_Priority = -1;
		m_vecCam[_Priority]->m_Registered = false;
	}

	m_vecCam[_Priority] = _Cam;
	m_vecCam[_Priority]->m_Registered = true;
}

void CRenderMgr::ResetEditorCamPos()
{
	m_EditorCam->Transform()->SetRelativePos(0, 0, 0);
	m_EditorCam->Transform()->SetRelativeRot(Vec3(0, 0, 0));
}


void CRenderMgr::Tick()
{
	// 광원 데이터 바인딩
	CLightMgr::GetInst()->Binding();

	// Global 데이터 바인딩
	static CConstBuffer* pGlobal = CDevice::GetInst()->GetConstBuffer(CB_TYPE::GLOBAL);
	pGlobal->SetData(&g_global, sizeof(tGlobal));
	pGlobal->Binding();
	pGlobal->Binding_CS();

	m_EditorCam->Tick();
	m_EditorCam->FinalTick();

	m_UICam->Tick();
	if (m_CurrentCam)
	{
		m_UICam->Transform()->SetRelativePos(m_CurrentCam->GetOwner()->Transform()->GetRelativePos());
		m_UICam->Transform()->SetRelativeRot(m_CurrentCam->GetOwner()->Transform()->GetRelativeRotQuat());
	}
	m_UICam->FinalTick();

	// 렌더 타겟 클리어
	ClearMRT();

	// Main 렌더링	
	Render();

	// 최종 백버퍼 렌더링
	MergeRender();
}

void CRenderMgr::Render()
{
	// 출력 렌더타겟 및 출력 깊이타겟 설정
	m_arrMRT[MRT_TYPE::DEFERRED]->SetRenderTarget();

	// 현재 레벨이 없거나 Stop 이면 에디터 카메라를 사용, 아니면 레벨의 카메라 오브젝트를 사용.
	CLevel* pLevel = CLevelMgr::GetInst()->GetCurrentLevel();
	if (pLevel == nullptr || pLevel->GetState() == LEVEL_STATE::STOP)
	{
		m_CurrentCam = m_EditorCam->Camera();
		m_EditorCam->Camera()->Render();
	}
	else
	{
		for (size_t i = 0; i < m_vecCam.size(); ++i)
		{
			m_CurrentCam = m_vecCam[i];
			m_vecCam[i]->Render();
		}
	}
}

void CRenderMgr::UIRender()
{
	m_arrMRT[MRT_TYPE::MERGE]->SetRenderTarget();
	m_UICam->Camera()->Render();
}

void CRenderMgr::DebugRender()
{
	vector<tDebugShapeInfo>::iterator iter = m_vecDebugInfo.begin();
	for (; iter != m_vecDebugInfo.end(); )
	{
		// 디버그 모양에 따른 메쉬 선택
		switch ((*iter).Shape)
		{
		case DEBUG_SHAPE::RECT:
			m_DebugObject->GetRenderComponent()->SetMesh(CAssetMgr::GetInst()->Load<CMesh>(L"RectMesh_Debug", true));
			break;
		case DEBUG_SHAPE::CIRCLE:
			m_DebugObject->GetRenderComponent()->SetMesh(CAssetMgr::GetInst()->Load<CMesh>(L"CircleMesh_Debug", true));
			break;
		case DEBUG_SHAPE::LINE:
			break;
		case DEBUG_SHAPE::CUBE:
			break;
		case DEBUG_SHAPE::SPHERE:
			break;
		default:
			break;
		}

		// 깊이 판정에 따라서 DepthStencilState 를 골라줌		
		if ((*iter).DepthTest)
			m_DebugObject->GetRenderComponent()->GetMaterial()->GetShader()->SetDSType(DS_TYPE::NO_WRITE);
		else
			m_DebugObject->GetRenderComponent()->GetMaterial()->GetShader()->SetDSType(DS_TYPE::NO_TEST_NO_WIRITE);

		// 색상값을 재질을 통해서 전달
		m_DebugObject->GetRenderComponent()->GetMaterial()->SetScalarParam(VEC4_0, (*iter).Color);

		// 위치정보 세팅 및 월드행렬 계산
		if ((*iter).MatWorld == Matrix::Identity)
		{
			m_DebugObject->Transform()->SetRelativePos((*iter).WorldPos);
			m_DebugObject->Transform()->SetRelativeScale((*iter).WorldScale);
			m_DebugObject->Transform()->SetRelativeRot((*iter).WorldRotation);
			m_DebugObject->Transform()->FinalTick();
		}
		else
		{
			m_DebugObject->Transform()->SetWorldMat((*iter).MatWorld);
		}

		// 디버그 오브젝트 렌더링
		m_DebugObject->Render();

		// 노출 시간이 넘어선 정보는 제거한다.
		(*iter).CurTime += EngineDT;
		if ((*iter).Duration < (*iter).CurTime)
			iter = m_vecDebugInfo.erase(iter);
		else
			++iter;
	}
}

void CRenderMgr::MergeRender()
{
	// 출력 렌더타겟 및 출력 깊이타겟 설정
	m_arrMRT[MRT_TYPE::MERGE]->SetRenderTarget();

	m_MergeMtrl->SetTexParam(TEX_0, m_arrMRT[MRT_TYPE::DEFERRED]->GetRenderTarget(0));
	//m_BackBufferMtrl->SetTexParam(TEX_0, CDevice::GetInst()->GetLightRTT());

	m_MergeMtrl->Binding();

	m_MergeMesh->Render();

	// UI 렌더링
	UIRender();

	// Debug 렌더링
	DebugRender();

	// Physx 렌더링
	CPhysxMgr::GetInst()->Render();

	// ImGui 렌더링
	CImguiMgr::GetInst()->Render();

	// 커서 렌더링
	CKeyMgr::GetInst()->Render();
}

void CRenderMgr::ClearMRT()
{
	for (int i = 0; i < MRT_END; ++i)
	{
		if (m_arrMRT[i])
			m_arrMRT[i]->ClearTargets();
	}
}

void CRenderMgr::UnbindShaders()
{
	CONTEXT->VSSetShader(nullptr, nullptr, 0);
	CONTEXT->GSSetShader(nullptr, nullptr, 0);
	CONTEXT->DSSetShader(nullptr, nullptr, 0);
	CONTEXT->HSSetShader(nullptr, nullptr, 0);
	CONTEXT->PSSetShader(nullptr, nullptr, 0);
}
