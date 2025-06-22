#ifndef _PARTICLE_FUNC
#define _PARTICLE_FUNC

#include "value.fx"


#define PI 3.1415926535f

float3 GetRandom(float _NomalizedThreadID, in Texture2D _NoiseTex)
{
    float2 vUV = (float2) 0.f;
        
    vUV.x = _NomalizedThreadID + g_EngineTime * 0.1f;
    vUV.y = 0.5f + sin(vUV.x * 100.f * PI) * 0.5f;
        
    float3 vRandom = _NoiseTex.SampleLevel(g_sam_0, vUV, 0);
    return vRandom;
}


void ParticleInit(inout tParticle _Particle, in tParticleModule _Module
                , in Texture2D _NoiseTex, int _IsNoiseBind, float _NomalizedThreadID)
{
    // NoiseTexture 가 없으면 랜덤값은 전부 0 으로 본다.    
      
    
    // 파티클 랜덤 위치 설정    
    // Box
    if (0 == _Module.SpawnShape)
    {
        float3 vRandom = GetRandom(_NomalizedThreadID, _NoiseTex);
        
        // 랜덤 범위를 -0.5 ~ 0.5f 로 변경 후, 스폰영역 크기를 곱해서 위치값을 구한다.
        float3 vSpanwPos = (vRandom - 0.5f) * _Module.SpawnShapeScale;
        _Particle.LocalPos.xyz = vSpanwPos;
        
        // 구 좌표계 기준 랜덤한 단위벡터를 구한 뒤 속력을 곱하여 최종 속도 계산.
        float3 vRandomDir = float3(sin(vRandom.y * PI) * cos(vRandom.x * 4.f * PI), sin(vRandom.y * PI) * sin(vRandom.x * 4.f * PI), cos(vRandom.y * PI));
        float RotAngle = acos(dot(_Module.SpawnDir, vRandomDir) / (length(vRandomDir) * length(_Module.SpawnDir))) * _Module.SpawnDirRandomize;
        float3 RotAxis = normalize(cross(_Module.SpawnDir, vRandomDir));
        float4 RotQuat = float4(RotAxis * sin(RotAngle / 2.f), cos(RotAngle / 2.f));
        float3 FinalAngle = 2.0 * dot(RotQuat.xyz, _Module.SpawnDir) * RotQuat.xyz + (RotQuat.w * RotQuat.w - dot(RotQuat.xyz, RotQuat.xyz)) * _Module.SpawnDir + 2.0 * RotQuat.w * cross(RotQuat.xyz, _Module.SpawnDir);
        float vSpawnSpd = vRandom.z * (_Module.MaxSpeed - _Module.MinSpeed) + _Module.MinSpeed;
        _Particle.Velocity.xyz = normalize(FinalAngle) * vSpawnSpd;
        
    }
    
    // Sphere
    else if (1 == _Module.SpawnShape)
    {
        
    }
  
    // World 공간계
    if (1 == _Module.SpaceType)
    {
        _Particle.LocalPos.xyz += _Module.ObjectWorldPos;
    }
        
    // 파티클 Life 랜덤 설정
    _Particle.Age = 0.f;
    float3 vRandom = GetRandom(_NomalizedThreadID + 0.1f, _NoiseTex);
    _Particle.Life = clamp(vRandom.r, _Module.MinLife, _Module.MaxLife);
        
    // 파티클 초기 색 설정
    _Particle.Color = _Module.StartColor;
    
    // 파티클 초기 크기 설정
    float3 vScale = (_Module.MaxScale - _Module.MinScale) * vRandom.x + _Module.MinScale;
    _Particle.WorldScale.xyz = vScale;
}

#endif