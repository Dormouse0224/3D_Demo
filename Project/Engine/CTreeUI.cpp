#include "pch.h"
#include "CTreeUI.h"

// Tree Node

UINT TreeNode::g_NextTreeID = 0;

TreeNode::TreeNode(string _Name)
	: m_TreeID(g_NextTreeID++)
	, m_Data((DWORD_PTR)nullptr)
	, m_Parent(nullptr)
	, m_vecChild{}
	, m_Owner(nullptr)
{
	m_Name = _Name + "##" + to_string(m_TreeID);
}

TreeNode::TreeNode(string _Name, DWORD_PTR _Data, TreeNode* _Parent)
	: m_TreeID(g_NextTreeID++)
	, m_Data(_Data)
	, m_Parent(_Parent)
	, m_vecChild{}
	, m_Owner(_Parent->m_Owner)
{
	m_Name = _Name + "##" + to_string(m_TreeID);

	if (_Parent)
	{
		_Parent->m_vecChild.push_back(this);
	}
}

TreeNode::~TreeNode()
{
	Delete_Vec(m_vecChild);
}

void TreeNode::Render()
{
	ImGuiTreeNodeFlags flag = 0;

	if (m_vecChild.empty())
		flag |= ImGuiTreeNodeFlags_Leaf;


	if (ImGui::TreeNodeEx(m_Name.c_str(), flag))
	{
		DragCheck();
		DropCheck();

		// 트리 노드가 열려있을 때 클릭 감지
		DoubleClickCheck();

		for (auto child : m_vecChild)
		{
			child->Render();
		}

		ImGui::TreePop();
	}
	else
	{
		// 트리 노드가 닫혀있을 때 클릭 감지
		DoubleClickCheck();

		DragCheck();
		DropCheck();
	}

}

void TreeNode::Clear()
{
	Delete_Vec(m_vecChild);
}

void TreeNode::DoubleClickCheck()
{
	if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left) && ImGui::IsItemHovered())
	{
		if (m_Owner->GetOwnerUI() != nullptr && m_Owner->GetDoubleClickFunc() != nullptr)
			(m_Owner->GetOwnerUI()->*(m_Owner->GetDoubleClickFunc()))(m_Data);	
	}
}

void TreeNode::DragCheck()
{
	if (ImGui::BeginDragDropSource())
	{
		ImGui::SetDragDropPayload(to_str(m_Owner->GetName()).c_str(), &m_Data, sizeof(DWORD_PTR));
		ImGui::Text(m_Name.c_str());
		ImGui::EndDragDropSource();
	}
}

void TreeNode::DropCheck()
{
	if (ImGui::BeginDragDropTarget())
	{
		// 같은 UI 에서 드래그 된 노드만 수령
		const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(to_str(m_Owner->GetName()).c_str());

		if (nullptr != payload && m_Owner->GetOwnerUI() != nullptr && m_Owner->GetDragDropFunc() != nullptr)
		{
			(m_Owner->GetOwnerUI()->*(m_Owner->GetDragDropFunc()))(*static_cast<DWORD_PTR*>((payload->Data)), m_Data);
		}

		ImGui::EndDragDropTarget();
	}
}

// Tree UI

CTreeUI::CTreeUI(wstring _Name)
	: CImguiObject(_Name)
	, m_Root(nullptr)
	, m_ShowRoot(false)
	, m_DoubleClickFunc(nullptr)
{
	m_Root = new TreeNode(string(_Name.begin(), _Name.end()));
	m_Root->m_Owner = this;
}

CTreeUI::~CTreeUI()
{
	delete m_Root;
}

void CTreeUI::Clear()
{
	if (nullptr != m_Root)
	{
		Delete_Vec(m_Root->m_vecChild);
	}
}

void CTreeUI::Update()
{
}

void CTreeUI::Render()
{
	if (m_ShowRoot)
	{
		m_Root->Render();
	}
	else
	{
		for (auto child : m_Root->m_vecChild)
		{
			child->Render();
		}
	}
		
}

TreeNode* CTreeUI::AddChildNode(TreeNode* _Parent, string _Name, DWORD_PTR m_Data)
{
	TreeNode* pChild = new TreeNode(_Name);
	pChild->m_Data = m_Data;
	pChild->m_Parent = _Parent;
	pChild->m_Owner = _Parent->m_Owner;

	_Parent->m_vecChild.push_back(pChild);

	return pChild;
}

