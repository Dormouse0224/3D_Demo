#include "pch.h"
#include "CCamera.h"

#include "CDevice.h"
#include "CTransform.h"
#include "CRenderMgr.h"

#include "CLevelMgr.h"
#include "CLevel.h"
#include "CLayer.h"
#include "CGameObject.h"
#include "components.h"
#include "CGraphicShader.h"

CCamera::CCamera()
	: CComponent(COMPONENT_TYPE::CAMERA)
	, m_ProjType(PROJ_TYPE::ORTHOGRAPHIC)
	, m_ViewX(1.f)
	, m_ViewY(1.f)
	, m_FOV(XM_PI / 2.f)
	, m_Far(10000.f)
	, m_matView()
	, m_matProj()
	, m_Priority(-1)
	, m_Registered(false)
	, m_Zoom(1.f)
	, m_LayerCheck(0)
{
	Vec2 vResolution = CDevice::GetInst()->GetRenderResolution();
	m_ViewX = vResolution.x;
	m_ViewY = vResolution.y;
}


CCamera::CCamera(const CCamera& _Origin)
	: CComponent(_Origin)
	, m_ProjType(_Origin.m_ProjType)
	, m_ViewX(_Origin.m_ViewX)
	, m_ViewY(_Origin.m_ViewY)
	, m_FOV(_Origin.m_FOV)
	, m_Far(_Origin.m_Far)
	, m_matView(_Origin.m_matView)
	, m_matProj(_Origin.m_matProj)
	, m_Priority(_Origin.m_Priority)
	, m_Registered(false)
	, m_Zoom(_Origin.m_Zoom)
	, m_LayerCheck(_Origin.m_LayerCheck)
{
}

CCamera::~CCamera()
{
}

void CCamera::SetPriority(int _Priority)
{
	if (_Priority < 0)
		return;
	m_Priority = _Priority;
	CRenderMgr::GetInst()->RegisterCamera(this, _Priority);
}


void CCamera::FinalTick()
{
	if (!m_Registered && m_Priority >= 0)
		CRenderMgr::GetInst()->RegisterCamera(this, m_Priority);

	// 뷰행렬 계산
	// 이동
	Vec3 vCamPos = Transform()->GetRelativePos();
	Matrix matTrans = XMMatrixTranslation(-vCamPos.x, -vCamPos.y, -vCamPos.z);

	// 회전
	Vec3 vR = Transform()->GetWorldDir(DIR::RIGHT);
	Vec3 vU = Transform()->GetWorldDir(DIR::UP);
	Vec3 vF = Transform()->GetWorldDir(DIR::FRONT);

	Matrix matRot = XMMatrixIdentity();
	matRot._11 = vR.x;	matRot._12 = vU.x;	matRot._13 = vF.x;
	matRot._21 = vR.y;	matRot._22 = vU.y;	matRot._23 = vF.y;
	matRot._31 = vR.z;	matRot._32 = vU.z;	matRot._33 = vF.z;

	m_matView = matTrans * matRot;


	// 투영행렬 계산하기
	if (ORTHOGRAPHIC == m_ProjType)
	{
		m_matProj = XMMatrixOrthographicLH(m_ViewX / m_Zoom, m_ViewY / m_Zoom, 1.f, m_Far);
	}
	else
	{
		m_matProj = XMMatrixPerspectiveFovLH(m_FOV / m_Zoom, m_ViewX / m_ViewY, 1.f, m_Far);
	}
}

void CCamera::SetMatrix()
{
	// 카메라의 View, Proj 행렬을 세팅
	g_Trans.matView = m_matView;
	g_Trans.matProj = m_matProj;
}

void CCamera::Render()
{
	SetMatrix();

	// 물체 분류
	SortObject();

	// 물체 렌더링
	// Opaque
	for (size_t i = 0; i < m_vecOpaque.size(); ++i)
	{
		m_vecOpaque[i]->Render();
	}

	// Masked
	for (size_t i = 0; i < m_vecMasked.size(); ++i)
	{
		m_vecMasked[i]->Render();
	}

	// Transparent
	for (size_t i = 0; i < m_vecTransparent.size(); ++i)
	{
		m_vecTransparent[i].second->Render();
	}

	// Postprocess
	for (size_t i = 0; i < m_vecPostprocess.size(); ++i)
	{
		m_vecPostprocess[i]->Render();
	}

	// UI
	for (size_t i = 0; i < m_vecUI.size(); ++i)
	{
		m_vecUI[i]->Render();
	}

	// 오브젝트 렌더링이 끝난 후 분류용 벡터 클리어
	m_vecOpaque.clear();
	m_vecMasked.clear();
	m_vecTransparent.clear();
	m_vecPostprocess.clear();
	m_vecUI.clear();
}

void CCamera::Direct_Render(const vector<CGameObject*>& _vecObj)
{
	SetMatrix();

	for (CGameObject* pObj : _vecObj)
	{
		pObj->Render();
	}
}

int CCamera::Load(fstream& _Stream)
{
	if (!_Stream.is_open())
		return E_FAIL;

	_Stream.read(reinterpret_cast<char*>(&m_ProjType), sizeof(PROJ_TYPE));	// 투영 방법
	_Stream.read(reinterpret_cast<char*>(&m_ViewX), sizeof(float));			// 카메라 투영 가로길이
	_Stream.read(reinterpret_cast<char*>(&m_ViewY), sizeof(float));			// 카메라 투영 세로길이
	_Stream.read(reinterpret_cast<char*>(&m_FOV), sizeof(float));			// 시야각(FieldOfView)
	_Stream.read(reinterpret_cast<char*>(&m_Far), sizeof(float));			// 최대 시야거리
	_Stream.read(reinterpret_cast<char*>(&m_Priority), sizeof(int));		// 카메라 우선순위, -1 : 미등록 카메라, 0 : 메인 카메라, 1 ~> : 서브 카메라
	_Stream.read(reinterpret_cast<char*>(&m_LayerCheck), sizeof(UINT));		// 카메라가 렌더링할 레이어 비트설정

	//CRenderMgr::GetInst()->RegisterCamera(this, m_Priority);

	return S_OK;
}

int CCamera::Save(fstream& _Stream)
{
	// m_matView, m_matProj 는 매 틱마다 계산되므로 저장하지 않음
	// 물체 분류 용도의 벡터 m_vecOpaque, m_vecMasked, m_vecTransparent 는 매 프레임마다 분류작업이 이루어지므로 저장하지 않음

	if (!_Stream.is_open())
		return E_FAIL;

	_Stream.write(reinterpret_cast<char*>(&m_ProjType), sizeof(PROJ_TYPE));		// 투영 방법
	_Stream.write(reinterpret_cast<char*>(&m_ViewX), sizeof(float));			// 카메라 투영 가로길이
	_Stream.write(reinterpret_cast<char*>(&m_ViewY), sizeof(float));			// 카메라 투영 세로길이
	_Stream.write(reinterpret_cast<char*>(&m_FOV), sizeof(float));				// 시야각(FieldOfView)
	_Stream.write(reinterpret_cast<char*>(&m_Far), sizeof(float));				// 최대 시야거리
	_Stream.write(reinterpret_cast<char*>(&m_Priority), sizeof(int));			// 카메라 우선순위, -1 : 미등록 카메라, 0 : 메인 카메라, 1 ~> : 서브 카메라
	_Stream.write(reinterpret_cast<char*>(&m_LayerCheck), sizeof(UINT));		// 카메라가 렌더링할 레이어 비트설정


	return S_OK;
}


void CCamera::SortObject()
{
	CLevel* pCurLevel = CLevelMgr::GetInst()->GetCurrentLevel();
	if (!pCurLevel)
		return;

	for (UINT i = 0; i < (UINT)LAYER::END; ++i)
	{
		// Camera 가 Rendering 하지 않는 레이어는 거른다.	
		if (!(m_LayerCheck & (1 << i)))
			continue;

		// 레이어에 속한 물체들을 가져온다.
		CLayer* pLayer = pCurLevel->GetLayer(i);
		const vector<CGameObject*>& vecObjects = pLayer->GetObjects();

		for (size_t j = 0; j < vecObjects.size(); ++j)
		{
			// 레이어 안에있는 물체들 중에서 렌더링 기능이 없는 물체는 거른다.
			if (!IsRenderable(vecObjects[j]) && vecObjects[j]->GetComponent(COMPONENT_TYPE::UICOM) == nullptr)
				continue;

			if (IsRenderable(vecObjects[j]))
			{
				// 2. 오브젝트가 사용하는 쉐이더의 도메인에 따라서 분류한다.
				switch (vecObjects[j]->GetRenderComponent()->GetMaterial()->GetShader()->GetDomain())
				{
				case SHADER_DOMAIN::DOMAIN_QPAQUE:
					m_vecOpaque.push_back(vecObjects[j]);
					break;
				case SHADER_DOMAIN::DOMAIN_MASKED:
					m_vecMasked.push_back(vecObjects[j]);
					break;
				case SHADER_DOMAIN::DOMAIN_TRANSPARENT:
				{
					Vec4 vpos = XMVector4Transform(Vec4(vecObjects[j]->Transform()->GetWorldTrans(), 1.f), g_Trans.matView);
					m_vecTransparent.push_back(make_pair(vpos.z, vecObjects[j]));
				}
					break;
				case SHADER_DOMAIN::DOMAIN_POSTPROCESS:
					m_vecPostprocess.push_back(vecObjects[j]);
					break;
				case SHADER_DOMAIN::DOMAIN_UI:
					m_vecUI.push_back(vecObjects[j]);
					break;
				}
			}
			else
			{
				m_vecUI.push_back(vecObjects[j]);
			}
		}

		// TRANSPARENT 렌더링의 경우 View 공간 기준 먼 객체부터 렌더링해야 하므로 정렬 수행
		sort(m_vecTransparent.begin(), m_vecTransparent.end(), [](pair<float, CGameObject*> _a, pair<float, CGameObject*> _b) -> bool { return _a.first > _b.first; });
	}

}