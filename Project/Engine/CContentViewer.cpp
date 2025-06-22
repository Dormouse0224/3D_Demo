#include "pch.h"
#include "CContentViewer.h"

#include "CAssetMgr.h"
#include "CImguiMgr.h"

#include "CTreeUI.h"
#include "CInspector.h"

CContentViewer::CContentViewer(wstring _Name)
	: CImguiObject(_Name)
{
	m_AssetTree = new CTreeUI(L"ContentViewer");
	m_AssetTree->SetDoubleClickFunc((DELEGATE_1)&CContentViewer::ClickEventCallback, this);
}

CContentViewer::~CContentViewer()
{
	delete m_AssetTree;
}

void CContentViewer::Update()
{
	if (CAssetMgr::GetInst()->GetRenew())
		Renew();
}

void CContentViewer::Render()
{
	m_AssetTree->Render();
}

void CContentViewer::ClickEventCallback(DWORD_PTR _Target)
{
	if (_Target != 0)
	{
		CImguiMgr::GetInst()->GetInspector()->SetTarget(reinterpret_cast<CEntity*>(_Target), TARGET_TYPE::ASSET);
	}
}

void CContentViewer::Renew()
{
	m_AssetTree->Clear();

	for (int i = 0; i < static_cast<UINT>(ASSET_TYPE::ASSET_END); ++i)
	{
		TreeNode* pNode = m_AssetTree->AddChildNode(m_AssetTree->GetRoot(), to_str(ASSET_TYPE_WSTR[i]), 0);
		for (auto& pair : CAssetMgr::GetInst()->GetAssets((ASSET_TYPE)i))
		{
			AssetPtr<CAsset> pAsset = pair.second;
			m_AssetTree->AddChildNode(pNode, to_str(pAsset.Get()->GetName()), (DWORD_PTR)pAsset.Get());
		}
	}
}
