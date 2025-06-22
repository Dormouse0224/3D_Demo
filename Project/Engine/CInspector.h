#pragma once
#include "CImguiObject.h"

enum class TARGET_TYPE
{
    GAMEOBJECT,
    ASSET,
};

class CGameObject;
class CComponentUI;
class CAsset;
class CAssetUI;

class CInspector :
    public CImguiObject
{
public:
    CInspector(wstring _Name);
    ~CInspector();
    CLONE_DISABLE(CInspector);

private:
    TARGET_TYPE         m_Type;
    CGameObject*        m_TargetObj;
    CComponentUI*       m_ComponentUI[(UINT)COMPONENT_TYPE::COMPONENT_END];
    AssetPtr<CAsset>    m_TargetAsset;
    CAssetUI*           m_AssetUI[(UINT)ASSET_TYPE::ASSET_END];


public:
    virtual void Update();
    virtual void Render();

    CGameObject* GetTargetObject() { return m_TargetObj; }

    void SetTargetObject(CGameObject* _Target) { m_TargetObj = _Target; }

    void SetTarget(CEntity* _Obj, TARGET_TYPE _Type);
};

