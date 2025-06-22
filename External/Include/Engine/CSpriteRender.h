#pragma once
#include "CRenderComponent.h"

#include "CSprite.h"

class CSpriteRender :
    public CRenderComponent
{
public:
    CSpriteRender();
    ~CSpriteRender();
    CLONE(CSpriteRender);

private:
    AssetPtr<CSprite>    m_Sprite;

public:
    AssetPtr<CSprite> GetSprite() { return m_Sprite; }

    void SetSprite(AssetPtr<CSprite> _Sprite) { m_Sprite = _Sprite; }


private:
    void CreateSpriteMaterial();

public:
    virtual void FinalTick() override;
    virtual void Render() override;

    virtual int Load(fstream& _Stream) override;
    virtual int Save(fstream& _Stream) override;

};

