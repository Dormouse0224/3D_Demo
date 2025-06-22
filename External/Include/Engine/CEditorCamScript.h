#pragma once
#include "CScript.h"
class CEditorCamScript :
    public CScript
{
public:
    CEditorCamScript();
    ~CEditorCamScript();
    CLONE(CEditorCamScript);
private:
    float   m_CamSpeedLin;
    float   m_CamSpeedAng;

public:
    virtual void Tick() override;

    virtual int Load(fstream& _Stream) override;
    virtual int Save(fstream& _Stream) override;

private:
    void Move_Perspective();
    void Move_OrthoGraphic();

};