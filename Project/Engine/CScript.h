#pragma once
#include "CComponent.h"

#include "CComponentMgr.h"

class CGameObject;

class CScript :
    public CComponent
{
public:
    CScript();
    CScript(const CScript& _Origin);
    ~CScript();
    virtual CScript* Clone() = 0;

public:
    virtual void Tick() = 0;
    virtual void FinalTick() final {}
    virtual int Save(fstream& _Stream) = 0;
    virtual int Load(fstream& _Stream) = 0;
    void Render();
    virtual void Render_Script();
};

