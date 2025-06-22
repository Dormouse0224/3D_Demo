#pragma once
#include "CImguiObject.h"

class CTreeUI;
class CGameObject;
struct TreeNode;

class COutliner :
    public CImguiObject
{
public:
    COutliner(wstring _Name);
    ~COutliner();
    CLONE_DISABLE(COutliner);

private:
    CTreeUI* m_ObjTree;

public:
    virtual void Update();
    virtual void Render();

    CTreeUI* GetObjTree() { return m_ObjTree; }

    void DoubleClickEvent(DWORD_PTR _Obj);
    void DragDropEvent(DWORD_PTR _Drag, DWORD_PTR _Drop);

private:
    void Renew();
    void AddObject(CGameObject* _Obj, TreeNode* _ParentNode);
};
