#include "pch.h"
#include "CRenderComponent.h"

#include "CAssetMgr.h"
#include "CLevel.h"
#include "CLevelMgr.h"
#include "CTransform.h"

CRenderComponent::CRenderComponent(COMPONENT_TYPE _Type)
	: CComponent(_Type)
    , m_Mesh(nullptr)
    , m_vecMtrls{}
    , m_FrustumCull(true)
    , m_ShadowMtrl(nullptr)
{
    m_ShadowMtrl = CAssetMgr::GetInst()->Load<CMaterial>(L"EA_ShadowMtrl", true);
}

CRenderComponent::CRenderComponent(const CRenderComponent& _Other)
	: CComponent(_Other)
	, m_Mesh(_Other.m_Mesh)
    , m_vecMtrls(_Other.m_vecMtrls)
    , m_FrustumCull(_Other.m_FrustumCull)
    , m_ShadowMtrl(_Other.m_ShadowMtrl)
{
}

CRenderComponent::~CRenderComponent()
{
}

void CRenderComponent::SetMaterial(AssetPtr<CMaterial> _Mtrl, UINT _Idx)
{
    if (m_vecMtrls.size() <= _Idx)
    {
        m_vecMtrls.resize(_Idx + 1);
    }

    m_vecMtrls[_Idx].pSharedMtrl = _Mtrl;
    if (m_vecMtrls[_Idx].pDynamicMtrl != nullptr)
        m_vecMtrls[_Idx].pDynamicMtrl = m_vecMtrls[_Idx].pSharedMtrl->Clone();
}

void CRenderComponent::SetUsingDynamic(bool _UsingDynamic, UINT _Idx)
{
    if (m_vecMtrls.size() > _Idx)
        m_vecMtrls[_Idx].bUsingDynamic = _UsingDynamic;
}

AssetPtr<CMaterial> CRenderComponent::GetMaterial(UINT _Idx)
{
    if (m_vecMtrls.size() <= _Idx)
        return nullptr;

    // 현재 레벨이 Play 상태이고, 동적 재질을 사용하도록 설정된 경우, 동적 재질을 반환
    if (CLevelMgr::GetInst()->GetCurrentLevel() != nullptr && LEVEL_STATE::PLAY == CLevelMgr::GetInst()->GetCurrentLevel()->GetState()
        && !CAssetMgr::GetInst()->GetLoading() && m_vecMtrls[_Idx].bUsingDynamic)
    {
        if (m_vecMtrls[_Idx].pDynamicMtrl != nullptr)
            m_vecMtrls[_Idx].pDynamicMtrl = m_vecMtrls[_Idx].pSharedMtrl->Clone();
        return m_vecMtrls[_Idx].pDynamicMtrl;
    }
    else
    {
        return m_vecMtrls[_Idx].pSharedMtrl;
    }
}

AssetPtr<CMaterial> CRenderComponent::GetSharedMaterial(UINT _Idx)
{
    if (m_vecMtrls.size() > _Idx)
        return m_vecMtrls[_Idx].pSharedMtrl;
}

bool CRenderComponent::GetUsingDynamic(UINT _Idx)
{
    if (m_vecMtrls.size() > _Idx)
        return m_vecMtrls[_Idx].bUsingDynamic;
}

void CRenderComponent::RenderShadow(LIGHT_TYPE _Type, int _Cascade)
{
    if (m_ShadowMtrl.Get() == nullptr)
        return;

    Transform()->Binding();
    m_ShadowMtrl->SetConstParam(INT_0, (int)_Type);
    m_ShadowMtrl->SetConstParam(INT_1, _Cascade);

    m_ShadowMtrl->Binding();
    m_Mesh->Render();
}

int CRenderComponent::RenderCom_Load(fstream& _Stream)
{
	if (!_Stream.is_open())
		return E_FAIL;

	std::wstring MeshName = {};
	int size = 0;
	_Stream.read(reinterpret_cast<char*>(&size), sizeof(int));
	if (size > 0)
	{
		MeshName.resize(size);
		_Stream.read(reinterpret_cast<char*>(MeshName.data()), sizeof(wchar_t) * size);
		m_Mesh = CAssetMgr::GetInst()->Load<CMesh>(MeshName);
	}

    size = 0;
    _Stream.read(reinterpret_cast<char*>(&size), sizeof(int));
    for (int i = 0; i < size; ++i)
    {
        tMtrlSet mtrl = {};
        wstring mtrlName = {};
        LoadWString(mtrlName, _Stream);
        mtrl.pSharedMtrl = CAssetMgr::GetInst()->Load<CMaterial>(mtrlName);
        _Stream.read(reinterpret_cast<char*>(&mtrl.bUsingDynamic), sizeof(bool));
        m_vecMtrls.push_back(mtrl);
    }

    _Stream.read(reinterpret_cast<char*>(&m_FrustumCull), sizeof(bool));

	return S_OK;
}

int CRenderComponent::RenderCom_Save(fstream& _Stream)
{
	if (!_Stream.is_open())
		return E_FAIL;

	int size = 0;
	if (m_Mesh.Get())
	{
		std::wstring MeshName = m_Mesh->GetName();
		size = MeshName.size();
		_Stream.write(reinterpret_cast<char*>(&size), sizeof(int));
		_Stream.write(reinterpret_cast<char*>(MeshName.data()), sizeof(wchar_t) * size);
	}
	else
		_Stream.write(reinterpret_cast<char*>(&size), sizeof(int));

    size = m_vecMtrls.size();
    _Stream.write(reinterpret_cast<char*>(&size), sizeof(int));
    for (int i = 0; i < size; ++i)
    {
        SaveWString(m_vecMtrls[i].pSharedMtrl->GetName(), _Stream);
        _Stream.write(reinterpret_cast<char*>(&m_vecMtrls[i].bUsingDynamic), sizeof(bool));
    }

    _Stream.write(reinterpret_cast<char*>(&m_FrustumCull), sizeof(bool));

	return S_OK;
}
