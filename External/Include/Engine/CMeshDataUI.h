#pragma once
#include "CAssetUI.h"

class CMeshDataUI :
    public CAssetUI
{
public:
    CMeshDataUI();
    ~CMeshDataUI();

private:

public:
    virtual void Update_Ast() override;
    virtual void Render_Ast() override;
};

