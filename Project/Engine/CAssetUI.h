#pragma once
#include "CImguiObject.h"

#include "CAsset.h"

class CAssetUI :
    public CImguiObject
{
public:
    CAssetUI(wstring _Name);
    ~CAssetUI();
    CLONE_DISABLE(CAssetUI)

protected:
    AssetPtr<CAsset> m_TargetAsset;

public:
    AssetPtr<CAsset> GetTarget() { return m_TargetAsset; }

    void SetTarget(AssetPtr<CAsset> _Obj) { m_TargetAsset = _Obj; }

    virtual void Update() final;
    virtual void Render() final;

private:
    virtual void Update_Ast() = 0;
    virtual void Render_Ast() = 0;
};

