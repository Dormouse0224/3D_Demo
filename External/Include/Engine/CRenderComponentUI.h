#pragma once
#include "CComponentUI.h"
#include "assets.h"
#include "CGameObject.h"
#include "CRenderComponent.h"

class CRenderComponentUI :
    public CComponentUI
{
public:
    CRenderComponentUI(COMPONENT_TYPE _Type);
    ~CRenderComponentUI();

protected:
    void Update_RCom();
    void Render_RCom();

};

