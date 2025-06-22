#pragma once
#include "CEntity.h"

#include "CGameObject.h"

#define GET_OTHER_COMPONENT(Type) class C##Type* Type() { return m_Owner->Type(); }

class CComponent :
    public CEntity
{
    friend class CGameObject;
public:
    CComponent(COMPONENT_TYPE _Type);
    CComponent(const CComponent& _Origin);
    ~CComponent();
    virtual CComponent* Clone() = 0;

private:
    const COMPONENT_TYPE    m_Type;
    CGameObject* m_Owner;

public:
    COMPONENT_TYPE GetType() { return m_Type; }
    CGameObject* GetOwner() { return m_Owner; }

protected:
    GET_OTHER_COMPONENT(Transform);
    GET_OTHER_COMPONENT(Camera);
    GET_OTHER_COMPONENT(PhysxActor);
    GET_OTHER_COMPONENT(FSM);
    GET_OTHER_COMPONENT(UICom);
    GET_OTHER_COMPONENT(Light);

    GET_OTHER_COMPONENT(MeshRender);
    GET_OTHER_COMPONENT(SpriteRender);
    GET_OTHER_COMPONENT(FlipbookRender);
    GET_OTHER_COMPONENT(TileRender);
    GET_OTHER_COMPONENT(ParticleRender);

    class CRenderComponent* RenderComponent() { return m_Owner->GetRenderComponent(); }

public:
    virtual void Begin() {};
    virtual void End() {};
    virtual void Tick() {};
    virtual void FinalTick() = 0;

    virtual int Save(fstream& _Stream) = 0;
    virtual int Load(fstream& _Stream) = 0;
};

