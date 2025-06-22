#pragma once
#include <physx/PxSimulationEventCallback.h>


// Extracting Contact Information

class CSimulationEvent :
    public PxSimulationEventCallback
{
public:


    virtual void onConstraintBreak(PxConstraintInfo* constraints, PxU32 count) override;


    virtual void onWake(PxActor** actors, PxU32 count) override;


    virtual void onSleep(PxActor** actors, PxU32 count) override;

    // 충돌체가 충돌 시 호출
    virtual void onContact(const PxContactPairHeader& pairHeader, const PxContactPair* pairs, PxU32 nbPairs) override;

    // 감지체가 출동 시 호출
    virtual void onTrigger(PxTriggerPair* pairs, PxU32 count) override;


    virtual void onAdvance(const PxRigidBody* const* bodyBuffer, const PxTransform* poseBuffer, const PxU32 count) override;

};

