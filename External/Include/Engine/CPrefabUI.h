#pragma once
#include "CAssetUI.h"

class CGameObject;

class CPrefabUI :
    public CAssetUI
{
public:
    CPrefabUI();
    ~CPrefabUI();

private:

public:
    virtual void Update_Ast();
    virtual void Render_Ast();
    void ObjectInfo(CGameObject* _Obj);
};

