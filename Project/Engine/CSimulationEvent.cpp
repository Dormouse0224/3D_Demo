#include "pch.h"
#include "CSimulationEvent.h"

#include "CGameObject.h"
#include "CPhysxActor.h"
#include "CLevelMgr.h"

// 	물리적인 제약(제한)이 무너졌을 때 호출됨 (예: hinge가 부러짐 등).
void CSimulationEvent::onConstraintBreak(PxConstraintInfo* constraints, PxU32 count)
{
}

// 객체가 슬립 상태에서 깨어날 때 호출됨.
void CSimulationEvent::onWake(PxActor** actors, PxU32 count)
{
}

// 객체가 다시 슬립 상태로 들어갈 때 호출됨.
void CSimulationEvent::onSleep(PxActor** actors, PxU32 count)
{
}

/// <summary>
/// 두 개의 물리 객체가 충돌했을 때 호출되며, 충돌한 객체 정보와 충돌 쌍 정보가 전달됩니다.
/// </summary>
/// <param name="pairHeader">충돌한 두 개의 Actor(객체)에 대한 기본 정보를 포함하는 구조체</param>
/// <param name="pairs">충돌한 개별 Shape(충돌면)들의 정보 배열</param>
/// <param name="nbPairs">이번 onContact 호출에서 전달된 pairs 배열의 개수(충돌한 Shape 쌍의 개수)</param>
void CSimulationEvent::onContact(const PxContactPairHeader& pairHeader, const PxContactPair* pairs, PxU32 nbPairs)
{
    for (PxU32 i = 0; i < nbPairs; i++)
    {
        const PxContactPair& cp = pairs[i];

        CGameObject* go_0 = reinterpret_cast<CGameObject*>(pairHeader.actors[0]->userData);
        CGameObject* go_1 = reinterpret_cast<CGameObject*>(pairHeader.actors[1]->userData);
        if (go_0->IsDead() || go_1->IsDead())
            return;

        if (cp.events & PxPairFlag::eNOTIFY_TOUCH_FOUND)
        {
            // 충돌이 시작되었을 때
            go_0->PhysxActor()->ContactBegin(go_1, cp);
            go_1->PhysxActor()->ContactBegin(go_0, cp);
        }

        if (cp.events & PxPairFlag::eNOTIFY_TOUCH_PERSISTS)
        {
            // 충돌이 계속되고 있을 때 (매 프레임마다 호출됨)
            go_0->PhysxActor()->ContactTick(go_1, cp);
            go_1->PhysxActor()->ContactTick(go_0, cp);
        }

        if (cp.events & PxPairFlag::eNOTIFY_TOUCH_LOST)
        {
            // 충돌이 끝났을 때
            go_0->PhysxActor()->ContactEnd(go_1, cp);
            go_1->PhysxActor()->ContactEnd(go_0, cp);
        }
    }
}

// Trigger 형태의 Collider에 다른 객체가 들어오거나 나갈 때 호출됨.
void CSimulationEvent::onTrigger(PxTriggerPair* pairs, PxU32 count)
{
    for (PxU32 i = 0; i < count; ++i)
    {
        const PxTriggerPair& pair = pairs[i];

        // 두 actor 모두 유효해야 하고, userData가 설정되어 있어야 함
        if (!pair.triggerActor || !pair.otherActor)
            continue;

        CGameObject* triggerGO = reinterpret_cast<CGameObject*>(pair.triggerActor->userData);
        CGameObject* otherGO = reinterpret_cast<CGameObject*>(pair.otherActor->userData);

        if (!triggerGO || !otherGO)
            continue;

        if (triggerGO->IsDead() || otherGO->IsDead())
            continue;

        if (pair.status & PxPairFlag::eNOTIFY_TOUCH_FOUND)
        {
            // 트리거에 들어갔을 때
            triggerGO->PhysxActor()->TriggerBegin(otherGO);
            otherGO->PhysxActor()->TriggerBegin(triggerGO);
        }

        if (pair.status & PxPairFlag::eNOTIFY_TOUCH_LOST)
        {
            // 트리거에서 나왔을 때
            triggerGO->PhysxActor()->TriggerEnd(otherGO);
            otherGO->PhysxActor()->TriggerEnd(triggerGO);
        }
    }
}

// continuous collision detection 등의 정보가 업데이트될 때 호출됨.
void CSimulationEvent::onAdvance(const PxRigidBody* const* bodyBuffer, const PxTransform* poseBuffer, const PxU32 count)
{
}

