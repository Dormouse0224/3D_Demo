#pragma once
#include "CImguiObject.h"

// Tree Node

struct TreeNode
{
    friend class CTreeUI;
public:
    TreeNode(string _Name);
    TreeNode(string _Name, DWORD_PTR _Data, TreeNode* _Parent);
    ~TreeNode();

public:
    static UINT g_NextTreeID;

private:
    string              m_Name;
    UINT                m_TreeID;
    DWORD_PTR           m_Data;
    TreeNode*           m_Parent;
    vector<TreeNode*>   m_vecChild;
    CTreeUI*            m_Owner;

public:
    void Render();
    void AddChild(TreeNode* _Child) { m_vecChild.push_back(_Child); }
    void Clear();
    void DoubleClickCheck();
    void DragCheck();
    void DropCheck();
};


// Tree UI Class

class CTreeUI :
    public CImguiObject
{
public:
    CTreeUI(wstring _Name);
    ~CTreeUI();
    CLONE_DISABLE(CTreeUI);

private:
    TreeNode*       m_Root;
    bool            m_ShowRoot;
    CImguiObject*   m_OwnerUI;
    DELEGATE_1      m_DoubleClickFunc;
    DELEGATE_2      m_DragDropFunc;

public:
    void Clear();
    virtual void Update();
    virtual void Render();

    TreeNode* GetRoot() { return m_Root; }
    DELEGATE_1 GetDoubleClickFunc() { return m_DoubleClickFunc; }
    DELEGATE_2 GetDragDropFunc() { return m_DragDropFunc; }
    CImguiObject* GetOwnerUI() { return m_OwnerUI; }

    /// <summary>
    /// 노드를 더블클릭 시 호출될 이벤트 콜백 함수를 등록합니다.
    /// 이벤트 콜백 함수는 입력값으로 노드의 DWORD_PTR 데이터를 받습니다.
    /// </summary>
    /// <param name="_SelectFunc">이벤트 콜백 함수</param>
    /// <param name="_Owner">함수 호출 객체</param>
    void SetDoubleClickFunc(DELEGATE_1  _SelectFunc, CImguiObject* _Owner) { m_DoubleClickFunc = _SelectFunc; m_OwnerUI = _Owner; }
    void SetDragDropFunc(DELEGATE_2  _DragDropFunc, CImguiObject* _Owner) { m_DragDropFunc = _DragDropFunc; m_OwnerUI = _Owner; }

    TreeNode* AddChildNode(TreeNode* _Parent, string _Name, DWORD_PTR m_Data);

};

