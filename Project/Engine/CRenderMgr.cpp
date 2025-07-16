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
    : m_vecCam{}
    , m_DebugObject(nullptr)
    , m_EditorCam(nullptr)
    , m_UICam(nullptr)
    , m_CurrentCam(nullptr)
    , m_vecDebugInfo{}
    , m_arrMRT{}
    , m_MergeMesh(nullptr)
    , m_MergeMtrl(nullptr)
    , m_MergeMode(-1)
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
	assert(_Priority >= 0);	// Priority �� 0 �̻��̾�� �մϴ�.

	if (m_vecCam.size() <= _Priority)
	{
		m_vecCam.resize(_Priority + 1);
	}

	// �Է¹��� priority �ڸ��� �ٸ� ī�޶� �̹� �ִ� ���, ���� ī�޶� priority -1 �� ���� �� �ڸ��� ���� ����.
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
	// Global ������ ���ε�
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

	// ���� Ÿ�� Ŭ����
	ClearMRT();

	// Main ������	
	MainRender();

    // Light ������
    LightRender();

	// ���� ����� ������
	MergeRender();
}

void CRenderMgr::MainRender()
{
	// ��� ����Ÿ�� �� ��� ����Ÿ�� ����
	m_arrMRT[MRT_TYPE::DEFERRED]->SetRenderTarget();

	// ���� ������ ���ų� Stop �̸� ������ ī�޶� ���, �ƴϸ� ������ ī�޶� ������Ʈ�� ���.
	CLevel* pLevel = CLevelMgr::GetInst()->GetCurrentLevel();
	if (pLevel == nullptr || pLevel->GetState() == LEVEL_STATE::STOP)
	{
		m_CurrentCam = m_EditorCam->Camera();
        m_CurrentCam->Render();
	}
	else
	{
		for (size_t i = 0; i < m_vecCam.size(); ++i)
		{
			m_CurrentCam = m_vecCam[i];
            m_CurrentCam->Render();
		}
	}
}

void CRenderMgr::LightRender()
{
    // ���� ������ ���ε�
    CLightMgr::GetInst()->Binding();

    CLightMgr::GetInst()->Render();
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
		// ����� ��翡 ���� �޽� ����
		switch ((*iter).Shape)
		{
		case DEBUG_SHAPE::RECT:
			m_DebugObject->GetRenderComponent()->SetMesh(CAssetMgr::GetInst()->Load<CMesh>(L"EA_RectMesh_Debug", true));
            m_DebugObject->GetRenderComponent()->GetMaterial()->SetShader(CAssetMgr::GetInst()->FindAsset<CGraphicShader>(L"EA_DebugShapeShader"));
			break;
		case DEBUG_SHAPE::CIRCLE:
			m_DebugObject->GetRenderComponent()->SetMesh(CAssetMgr::GetInst()->Load<CMesh>(L"EA_CircleMesh_Debug", true));
            m_DebugObject->GetRenderComponent()->GetMaterial()->SetShader(CAssetMgr::GetInst()->FindAsset<CGraphicShader>(L"EA_DebugShapeShader"));
			break;
		case DEBUG_SHAPE::LINE:
            m_DebugObject->GetRenderComponent()->SetMesh(CAssetMgr::GetInst()->Load<CMesh>(L"EA_LineMesh", true));
            m_DebugObject->GetRenderComponent()->GetMaterial()->SetShader(CAssetMgr::GetInst()->FindAsset<CGraphicShader>(L"EA_DebugShapeShader"));
			break;
		case DEBUG_SHAPE::CUBE:
            m_DebugObject->GetRenderComponent()->SetMesh(CAssetMgr::GetInst()->Load<CMesh>(L"EA_CubeMesh_Debug", true));
            m_DebugObject->GetRenderComponent()->GetMaterial()->SetShader(CAssetMgr::GetInst()->FindAsset<CGraphicShader>(L"EA_DebugShapeShader"));
			break;
		case DEBUG_SHAPE::SPHERE:
            m_DebugObject->GetRenderComponent()->SetMesh(CAssetMgr::GetInst()->Load<CMesh>(L"EA_SphereMesh", true));
            m_DebugObject->GetRenderComponent()->GetMaterial()->SetShader(CAssetMgr::GetInst()->FindAsset<CGraphicShader>(L"EA_DebugShapeShader_Sphere"));
			break;
		default:
			break;
		}

		// ���� ������ ���� DepthStencilState �� �����		
		if ((*iter).DepthTest)
			m_DebugObject->GetRenderComponent()->GetMaterial()->GetShader()->SetDSType(DS_TYPE::NO_WRITE);
		else
			m_DebugObject->GetRenderComponent()->GetMaterial()->GetShader()->SetDSType(DS_TYPE::NO_TEST_NO_WIRITE);

		// ������ ������ ���ؼ� ����
		m_DebugObject->GetRenderComponent()->GetMaterial()->SetConstParam(VEC4_0, (*iter).Color);

		// ��ġ���� ���� �� ������� ���
		if ((*iter).MatWorld == Matrix::Identity)
		{
			m_DebugObject->Transform()->SetRelativePos((*iter).WorldPos);
			m_DebugObject->Transform()->SetRelativeScale((*iter).WorldScale);
			m_DebugObject->Transform()->SetRelativeRot((*iter).WorldRotation);
			//m_DebugObject->Transform()->FinalTick();
		}
		else
		{
			m_DebugObject->Transform()->SetWorldMat((*iter).MatWorld);
		}

		// ����� ������Ʈ ������
        m_CurrentCam->FinalTick();
        m_DebugObject->FinalTick();
        vector<CGameObject*> vec = { m_DebugObject };
        m_CurrentCam->Camera()->Direct_Render(vec);

		// ���� �ð��� �Ѿ ������ �����Ѵ�.
		(*iter).CurTime += EngineDT;
		if ((*iter).Duration < (*iter).CurTime)
			iter = m_vecDebugInfo.erase(iter);
		else
			++iter;
	}
}

void CRenderMgr::MergeRender()
{
	// ��� ����Ÿ�� �� ��� ����Ÿ�� ����
	m_arrMRT[MRT_TYPE::MERGE]->SetRenderTarget();

	m_MergeMtrl->SetTexParam(TEX_0, m_arrMRT[MRT_TYPE::DEFERRED]->GetRenderTarget(0));  // Albedo
    m_MergeMtrl->SetTexParam(TEX_1, m_arrMRT[MRT_TYPE::DEFERRED]->GetRenderTarget(1));  // Normal
    m_MergeMtrl->SetTexParam(TEX_2, m_arrMRT[MRT_TYPE::DEFERRED]->GetRenderTarget(2));  // Position
    m_MergeMtrl->SetTexParam(TEX_3, m_arrMRT[MRT_TYPE::DEFERRED]->GetRenderTarget(3));  // Effect
    m_MergeMtrl->SetTexParam(TEX_4, m_arrMRT[MRT_TYPE::LIGHT]->GetRenderTarget(0));     // Diffuse
    m_MergeMtrl->SetTexParam(TEX_5, m_arrMRT[MRT_TYPE::LIGHT]->GetRenderTarget(1));     // Specular
    m_MergeMtrl->SetConstParam(INT_0, m_MergeMode);
	m_MergeMtrl->Binding();

	m_MergeMesh->Render();

	// UI ������
	UIRender();

	// Debug ������
	DebugRender();

	// Physx ������
	CPhysxMgr::GetInst()->Render();

	// ImGui ������
	CImguiMgr::GetInst()->Render();

	// Ŀ�� ������
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

void CRenderMgr::UnbindShaders_CS()
{
    CONTEXT->CSSetShader(nullptr, nullptr, 0);
}

void CRenderMgr::UnbindResources()
{
    ID3D11ShaderResourceView* nullSRVs[D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT] = {};
    UINT count = D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT;

    // �� ���̴� ���������� SRV ����
    CONTEXT->VSSetShaderResources(0, count, nullSRVs); // Vertex Shader
    CONTEXT->PSSetShaderResources(0, count, nullSRVs); // Pixel Shader
    CONTEXT->GSSetShaderResources(0, count, nullSRVs); // Geometry Shader
    CONTEXT->HSSetShaderResources(0, count, nullSRVs); // Hull Shader
    CONTEXT->DSSetShaderResources(0, count, nullSRVs); // Domain Shader
}

void CRenderMgr::UnbindResources_CS()
{
    ID3D11ShaderResourceView* nullSRVs[D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT] = {};
    UINT count = D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT;

    CONTEXT->CSSetShaderResources(0, count, nullSRVs); // Compute Shader
}

void CRenderMgr::UnbindResources_CS_UAV()
{
    ID3D11UnorderedAccessView* nullUAVs[D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT] = {};
    UINT count = D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT;
    UINT i = -1;
    CONTEXT->CSSetUnorderedAccessViews(0, count, nullUAVs, &i); // Compute Shader
}