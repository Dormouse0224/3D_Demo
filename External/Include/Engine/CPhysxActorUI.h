#pragma once
#include "CComponentUI.h"
class CPhysxActorUI :
    public CComponentUI
{
public:
    CPhysxActorUI();
    ~CPhysxActorUI();

private:


public:
    virtual void Update_Com();
    virtual void Render_Com();

private:
    void EditColliderDescPopup(int _Idx);
};

