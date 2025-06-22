#pragma once
#include "CComponent.h"
#include "CPhysxMgr.h"

#include "CScript.h"

// 충돌 이벤트 발생 시 호출되는 함수
typedef void (CScript::*PxContactFunc)(CGameObject* _Other, const PxContactPair pair);

// 트리거 이벤트 발생 시 호출되는 함수
typedef void (CScript::*PxTriggerFunc)(CGameObject* _Other);

struct COLLIDER_DESC
{
    PxShapeFlags        ShapeFlag;
    PxReal              StaticFriction      = 0.f;
    PxReal              DynamicFriction     = 0.f;
    PxReal              Restitution         = 0.f;
    PxU32               FilterLayer_Self;
    PxU32               FilterLayer_Other;
};

enum class RIGID_TYPE
{
    KINEMATIC,
    DYNAMIC,
    STATIC,

    NONE,
};

enum LOCK_FLAG
{
    NONE        = 0,
    LINEAR_X    = (1<<0),
    LINEAR_Y    = (1<<1),
    LINEAR_Z    = (1<<2),
    ANGULAR_X   = (1<<3),
    ANGULAR_Y   = (1<<4),
    ANGULAR_Z   = (1<<5),

    LINEAR_ALL  = LINEAR_X | LINEAR_Y | LINEAR_Z,
    ANGULAR_ALL = ANGULAR_X | ANGULAR_Y | ANGULAR_Z,
};

extern wstring LOCK_FLAG_WSTR[];

class CPhysxActor :
    public CComponent
{
    friend class CPhysxMgr;
public:
    CPhysxActor();
    CPhysxActor(const CPhysxActor& _Other);
    ~CPhysxActor();
    CLONE(CPhysxActor);

private:
    PxRigidActor*           m_Body;
    RIGID_TYPE              m_Type;
    UINT                    m_LockFlag;
    bool                    m_Gravity;
    float                   m_Density;
    PxVec3                  m_InitVel;

    vector<PxShape*>        m_vecShape;
    vector<COLLIDER_DESC>   m_vecDesc;
    vector<PxVec3>          m_vecScale;
    vector<PxVec3>          m_vecOffset;

    // 복사 생성 시 Body 와 Collider 생성이 불가능(Owner Object 가 설정되지 않은 타이밍이기 때문)하므로, 이후 FinalTick 에서 마저 초기화를 진행한다.
    // 오직 복사 생성 시에만 true 가 입력되며, FinalTick 에서 추가 초기화 진행 후 false 가 된다.
    bool                    m_DelayedInit;

    CScript*                m_ContactBeginInst;
    CScript*                m_ContactTickInst;
    CScript*                m_ContactEndInst;
    PxContactFunc           m_ContactBegin;
    PxContactFunc           m_ContactTick;
    PxContactFunc           m_ContactEnd;

    set<CGameObject*>       m_setContactObj;

    CScript*                m_TriggerBeginInst;
    CScript*                m_TriggerEndInst;
    PxTriggerFunc           m_TriggerBegin;
    PxTriggerFunc           m_TriggerEnd;

public:
    virtual void Begin() {};
    virtual void Tick() {};
    virtual void FinalTick();

public:
    PxRigidActor* GetRigidBody() { return m_Body; }
    RIGID_TYPE GetRigidType() { return m_Type; }
    bool GetGravity() { return m_Gravity; }
    float GetDensity() { return m_Density; }
    int GetColliderCount() { return m_vecShape.size(); }
    COLLIDER_DESC GetColliderDesc(int _Idx) { if (_Idx < m_vecDesc.size()) { return m_vecDesc[_Idx]; } }
    PxVec3 GetColliderScale(int _Idx) { if (_Idx < m_vecScale.size()) { return m_vecScale[_Idx]; } }
    PxVec3 GetColliderOffset(int _Idx) { if (_Idx < m_vecOffset.size()) { return m_vecOffset[_Idx]; } }
    PxVec3 GetLinearVelocity();
    const set<CGameObject*>& GetContactObj() { return m_setContactObj; }

    void SetRigidBody(RIGID_TYPE _Type, UINT _LockFlag = 0, bool _Gravity = false, float _Density = 1.f, PxVec3 _InitVel = PxVec3(0.f));
    void SetRigidType(RIGID_TYPE _Type);
    void SetGravity(bool _Gravity);
    void SetDensity(float _Density);
    void SetColliderDesc(int _Idx, COLLIDER_DESC _Data);
    void SetColliderOffset(int _Idx, PxVec3 _Data);
    void SetLinearVelocity(PxVec3 _Vel);

    PxShape* AddCollider(COLLIDER_DESC _desc, PxVec3 _Scale = PxVec3(1.f), PxVec3 _Offset = PxVec3(0.f));
    void DeleteCollider(int _Idx);
    void DeleteCollider(PxShape* _Shape);
    void DeleteAllCollider();
    void UpdatePosition(Vec3 _Pos);
    void UpdateRotation(Vec4 _RotQuat);
    void CkeckLockFlag(LOCK_FLAG _Flag);

    void SetContactBegin(CScript* _Inst, PxContactFunc _Func) { m_ContactBeginInst = _Inst; m_ContactBegin = _Func; }
    void SetContactTick(CScript* _Inst, PxContactFunc _Func) { m_ContactTickInst = _Inst; m_ContactTick = _Func; }
    void SetContactEnd(CScript* _Inst, PxContactFunc _Func) { m_ContactEndInst = _Inst; m_ContactEnd = _Func; }

    void SetTriggerBegin(CScript* _Inst, PxTriggerFunc _Func) { m_TriggerBeginInst = _Inst; m_TriggerBegin = _Func; }
    void SetTriggerEnd(CScript* _Inst, PxTriggerFunc _Func) { m_TriggerEndInst = _Inst; m_TriggerEnd = _Func; }

    void ContactBegin(CGameObject* _Other, const PxContactPair pair);
    void ContactTick(CGameObject* _Other, const PxContactPair pair);
    void ContactEnd(CGameObject* _Other, const PxContactPair pair);

    void TriggerBegin(CGameObject* _Other);
    void TriggerEnd(CGameObject* _Other);

    virtual int Load(fstream& _Stream) override;
    virtual int Save(fstream& _Stream) override;

private:
    PxShape* AttachCollider(COLLIDER_DESC _desc, PxVec3 _Scale, PxVec3 _Offset);
};

