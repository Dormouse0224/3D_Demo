#pragma once

class CCamera;
class CGameObject;
class CMesh;
class CMaterial;
class CMRT;

class CRenderMgr
	: public Singleton<CRenderMgr>
{
	SINGLE(CRenderMgr);
private:
	// 현재 레벨에서 렌더링을 수행중인 카메라 벡터. 레벨이 생성될 때 카메라 컴포넌트를 소유한 오브젝트 배치 시,
	// SetPriority 함수에서 카메라의 Priority 값을 입력받고 RegisterCamera 함수를 통해 이 벡터에 등록된다.
	vector<CCamera*>		m_vecCam;
	CGameObject*			m_DebugObject;
	CGameObject*			m_EditorCam;
	CGameObject*			m_UICam;
	CCamera*				m_CurrentCam;

	vector<tDebugShapeInfo> m_vecDebugInfo;

	CMRT*					m_arrMRT[MRT_END];

	AssetPtr<CMesh>			m_MergeMesh;
	AssetPtr<CMaterial>		m_MergeMtrl;



public:
	void RegisterCamera(CCamera* _Cam, int _Priority);
	void AddDebugShapeInfo(const tDebugShapeInfo& _info) { m_vecDebugInfo.push_back(_info); }
	void ClearCamera() { m_vecCam.clear(); }
	void ResetEditorCamPos();
	CGameObject* GetUICam() { return m_UICam; }
	CGameObject* GetEditorCam() { return m_EditorCam; }
	CCamera* GetCurrentCam() { return m_CurrentCam; }

	void UnbindShaders();

public:
	void Init();
	void Tick();

private:
	void Render();
	void UIRender();
	void DebugRender();
	void MergeRender();

	int CreateBackBufferView();
	int CreateMRT();
	void ClearMRT();
};

