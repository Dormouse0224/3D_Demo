#ifndef _PARTICLE_TICK
#define _PARTICLE_TICK

#include "value.fx"
#include "particle_func.fx"

RWStructuredBuffer<tParticle> g_Buffer : register(u0);
RWStructuredBuffer<tSpawnCount> g_Spawn : register(u1);
StructuredBuffer<tParticleModule> g_Module : register(t20);

#define MAX_PARTICLE g_int_0



[numthreads(256, 1, 1)]
void CS_ParticleTick(int3 _ID : SV_DispatchThreadID)
{
    if (MAX_PARTICLE <= _ID.x)
        return;
    
    g_Buffer[_ID.x].PrevActive = g_Buffer[_ID.x].Active;
    
    // 스레드가 담당한 파티클이 비활성화 상태라면
    if (false == g_Buffer[_ID.x].Active)
    {
        while (g_Spawn[0].SpawnCount)
        {
            int SpawnCount = g_Spawn[0].SpawnCount;
            int Input = SpawnCount - 1;
            int Origin = 0;
        
            // Atomic 함수, 스레드가 하나씩 순차적으로 실행
            // dest 와 compareval 이 일치하면, val을 dest에 넣고 dest 에 있던 값을 originval에 넣음
            InterlockedCompareExchange(g_Spawn[0].SpawnCount, SpawnCount, Input, Origin);
            //InterlockedExchange(g_Spawn[0].SpawnCount, Input, Origin);
        
            // InterlockedCompareExchange 에서 성공한 스레드는 이전 dest 값(SpawnCount) 을 originval 에 받고, 실패한 스레드는 그대로 0
            if (Origin == SpawnCount)
            {
                g_Buffer[_ID.x].Active = true;
                break;
            }
        }
        
        // 활성화된 파티클 초기화 작업
        if (g_Buffer[_ID.x].Active)
        {
            float NormalizedThreadID = (float) _ID.x / (float) MAX_PARTICLE;
            ParticleInit(g_Buffer[_ID.x], g_Module[0], g_tex_0, g_btex_0, NormalizedThreadID);
        }
    }
    else
    {
        // 파티클의 수명이 다하면 비활성화 시킨다.
        g_Buffer[_ID.x].Age += g_DT;
        if (g_Buffer[_ID.x].Life < g_Buffer[_ID.x].Age)
        {
            g_Buffer[_ID.x].Active = false;
            return;
        }
        
        // Local 타입 파티클인 경우 위치 직접 조작
        if (g_Module[0].SpaceType == 0)
        {
            g_Buffer[_ID.x].LocalPos += g_Buffer[_ID.x].Velocity * g_DT;
        }
        
    }
}

#endif