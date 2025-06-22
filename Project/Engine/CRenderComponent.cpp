#include "pch.h"
#include "CRenderComponent.h"

#include "CAssetMgr.h"
#include "CLevel.h"
#include "CLevelMgr.h"

CRenderComponent::CRenderComponent(COMPONENT_TYPE _Type)
	: CComponent(_Type)
	, m_UsingDynamic(false)
{
}

CRenderComponent::CRenderComponent(const CRenderComponent& _Other)
	: CComponent(_Other)
	, m_Mesh(_Other.m_Mesh)
	, m_DefaultMtrl(_Other.m_DefaultMtrl)
	, m_UsingDynamic(false)
{
}

CRenderComponent::~CRenderComponent()
{
}

void CRenderComponent::SetDynamic(bool _UsingDynamic)
{
	if (_UsingDynamic)
	{
		if (LEVEL_STATE::PLAY != CLevelMgr::GetInst()->GetCurrentLevel()->GetState() || m_DefaultMtrl == nullptr)
			return;

		if (m_DynamicMtrl == nullptr)
		{
			m_DynamicMtrl = m_DefaultMtrl->Clone();
		}

		m_UsingDynamic = _UsingDynamic;
	}
	else
	{
		if (m_UsingDynamic)
		{
			delete m_DynamicMtrl.Get();
			m_UsingDynamic = _UsingDynamic;
		}
	}
}

AssetPtr<CMaterial> CRenderComponent::GetMaterial()
{
	if (CLevelMgr::GetInst()->GetCurrentLevel() != nullptr && LEVEL_STATE::PLAY == CLevelMgr::GetInst()->GetCurrentLevel()->GetState()
		&& !CAssetMgr::GetInst()->GetLoading())
		SetDynamic(true);
	//else
	//	SetDynamic(false);

	if (m_UsingDynamic)
		return m_DynamicMtrl;
	else
		return m_DefaultMtrl;
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

	std::wstring MtrlName = {};
	size = 0;
	_Stream.read(reinterpret_cast<char*>(&size), sizeof(int));
	if (size > 0)
	{
		MtrlName.resize(size);
		_Stream.read(reinterpret_cast<char*>(MtrlName.data()), sizeof(wchar_t) * size);
		m_DefaultMtrl = CAssetMgr::GetInst()->Load<CMaterial>(MtrlName);
	}

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

	size = 0;
	if (m_DefaultMtrl.Get())
	{
		std::wstring MtrlName = m_DefaultMtrl->GetName();
		size = MtrlName.size();
		_Stream.write(reinterpret_cast<char*>(&size), sizeof(int));
		_Stream.write(reinterpret_cast<char*>(MtrlName.data()), sizeof(wchar_t) * size);
	}
	else
		_Stream.write(reinterpret_cast<char*>(&size), sizeof(int));

	return S_OK;
}
