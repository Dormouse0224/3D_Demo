#ifndef _FUNC
#define _FUNC

#include "struct.fx"
#include "value.fx"

#define DIRECTIONAL 0
#define POINT 1
#define SPOT 2

void CalcLight(float3 _ViewPos, float3 _ViewNormal, tLightModule _Module, float _SpecularRatio, inout float3 _Diffuse, inout float3 _Specular)
{
    if (_Module.Type == DIRECTIONAL)
    {
        float3 Normal = normalize(_ViewNormal);
        float3 LightDir = normalize(mul(float4(_Module.Dir, 0.f), g_matView).xyz);
        float3 Diffuse = max(dot(Normal, -LightDir), 0) * _Module.Color;
        
        float3 ReflectDir = normalize(dot(Normal, -LightDir) * Normal * 2 + LightDir);
        float3 ViewDir = normalize(_ViewPos);
        float3 Specular = pow(max(dot(-ViewDir, ReflectDir), 0), _SpecularRatio) * _Module.Color;
        
        _Diffuse += Diffuse;
        _Specular += Specular;
    }
    else if (_Module.Type == POINT)
    {
        float3 LocalPos = mul(mul(float4(_ViewPos, 1.f), g_matInvView), g_matInvWorld).xyz;
        if (length(LocalPos) > 0.5f)
            return;
        
        float3 Normal = normalize(_ViewNormal);
        float3 vLightViewPos = mul(float4(_Module.WorldPos, 1.f), g_matView).xyz;
        float3 LightDir = normalize(_ViewPos - vLightViewPos);
        float3 Diffuse = max(dot(Normal, -LightDir), 0) * _Module.Color;
        
        float3 ReflectDir = normalize(dot(Normal, -LightDir) * Normal * 2 + LightDir);
        float3 ViewDir = normalize(_ViewPos);
        float3 Specular = pow(max(dot(-ViewDir, ReflectDir), 0), _SpecularRatio) * _Module.Color;
        
        float Ratio = saturate(cos((length(_ViewPos - vLightViewPos) / _Module.Radius) * (PI / 2.f)));
        _Diffuse += Diffuse * Ratio;
        _Specular += Specular * Ratio;
    }
    else if (_Module.Type == SPOT)
    {
        float3 LocalPos = mul(mul(float4(_ViewPos, 1.f), g_matInvView), g_matInvWorld).xyz;
        float Angle = acos(dot(normalize(LocalPos), float3(0.f, 0.f, 1.f))) * (180.f / PI);
        if (length(LocalPos) > 0.5f || Angle > _Module.Angle / 2.f)
            return;
        
        float3 Normal = normalize(_ViewNormal);
        float3 vLightViewPos = mul(float4(_Module.WorldPos, 1.f), g_matView).xyz;
        float3 LightDir = normalize(_ViewPos - vLightViewPos);
        float3 Diffuse = max(dot(Normal, -LightDir), 0) * _Module.Color;
        
        float3 ReflectDir = normalize(dot(Normal, -LightDir) * Normal * 2 + LightDir);
        float3 ViewDir = normalize(_ViewPos);
        float3 Specular = pow(max(dot(-ViewDir, ReflectDir), 0), _SpecularRatio) * _Module.Color;
        
        float Ratio = saturate(cos((length(_ViewPos - vLightViewPos) / _Module.Radius) * (PI / 2.f)));
        _Diffuse += Diffuse * Ratio;
        _Specular += Specular * Ratio;
    }

}

int IntersectsRay(float3 _Pos[3], float3 _vStart, float3 _vDir
                  , out float3 _CrossPos, out uint _Dist)
{
    // 삼각형 표면 방향 벡터
    float3 Edge[2] = { (float3) 0.f, (float3) 0.f };
    Edge[0] = _Pos[1] - _Pos[0];
    Edge[1] = _Pos[2] - _Pos[0];
    
    // 삼각형에 수직방향인 법선(Normal) 벡터
    float3 Normal = normalize(cross(Edge[0], Edge[1]));
    
    // 삼각형 법선벡터와 Ray 의 Dir 을 내적
    // 광선에서 삼각형으로 향하는 수직벡터와, 광선의 방향벡터 사이의 cos 값
    float NdotD = -dot(Normal, _vDir);
    
    float3 vStoP0 = _vStart - _Pos[0];
    float VerticalDist = dot(Normal, vStoP0); // 광선을 지나는 한점에서 삼각형 평면으로의 수직 길이
            
    // 광선이 진행하는 방향으로, 삼각형을 포함하는 평면까지의 거리
    float RtoTriDist = VerticalDist / NdotD;
        
    // 광선이, 삼각형을 포함하는 평면을 지나는 교점
    float3 vCrossPoint = _vStart + RtoTriDist * _vDir;
        
    // 교점이 삼각형 내부인지 테스트
    float3 P0toCross = vCrossPoint - _Pos[0];
    
    float3 Full = cross(Edge[0], Edge[1]);
    float3 U = cross(Edge[0], P0toCross);
    float3 V = cross(Edge[1], P0toCross);
       
    // 직선과 삼각형 평면의 교점이 삼각형 1번과 2번 사이에 존재하는지 체크
    //      0
    //     /  \
    //    1 -- 2    
    if (dot(U, Full) < 0.f || 0.f < dot(V, Full))
        return 0;
    
    // 교점이 삼각형 내부인지 체크
    if (length(Full) < length(U) + length(V))
        return 0;
        
    _CrossPos = vCrossPoint;
    _Dist = (uint) RtoTriDist;
    
    return 1;
}

float GetTessFactor(float _MinLevel, float _MaxLevel
                  , float _MinRange, float _MaxRange
                  , float3 _CamPos, float3 _Pos)
{
    float D = distance(_CamPos, _Pos);
    
    if (D < _MaxRange)
        return pow(2.f, _MaxLevel);
    else if (_MinRange < D)
        return pow(2.f, _MinLevel);
    else
    {
        //float fRatio = (D - _MinRange) / (_MaxRange - _MinRange);
        
        //float Level = _MinLevel + fRatio * (_MaxLevel - _MinLevel);
        float fRatio = 1.f - (D - _MaxRange) / abs(_MaxRange - _MinRange);
        
        float Level = 1.f + fRatio * (_MaxLevel - _MinLevel - 1.f);
        
        return pow(2.f, Level);
    }
}

matrix GetBoneMat(int _iBoneIdx, int _iRowIdx)
{
    return g_arrBoneMat[(g_iBoneCount * _iRowIdx) + _iBoneIdx];
}

void Skinning(inout float3 _vPos, inout float3 _vTangent, inout float3 _vBinormal, inout float3 _vNormal
    , inout float4 _vWeight, inout float4 _vIndices
    , int _iRowIdx)
{
    tSkinningInfo info = (tSkinningInfo) 0.f;

    if (_iRowIdx == -1)
        return;

    for (int i = 0; i < 4; ++i)
    {
        if (0.f == _vWeight[i])
            continue;

        matrix matBone = GetBoneMat((int) _vIndices[i], _iRowIdx);

        info.vPos += (mul(float4(_vPos, 1.f), matBone) * _vWeight[i]).xyz;
        info.vTangent += (mul(float4(_vTangent, 0.f), matBone) * _vWeight[i]).xyz;
        info.vBinormal += (mul(float4(_vBinormal, 0.f), matBone) * _vWeight[i]).xyz;
        info.vNormal += (mul(float4(_vNormal, 0.f), matBone) * _vWeight[i]).xyz;
    }

    _vPos = info.vPos;
    _vTangent = normalize(info.vTangent);
    _vBinormal = normalize(info.vBinormal);
    _vNormal = normalize(info.vNormal);
}


void Skinning_LocalPos(inout float3 _vPos, inout float4 _vWeight, inout float4 _vIndices, int _iRowIdx)
{
    tSkinningInfo info = (tSkinningInfo) 0.f;

    if (_iRowIdx == -1)
        return;

    for (int i = 0; i < 4; ++i)
    {
        if (0.f == _vWeight[i])
            continue;

        matrix matBone = GetBoneMat((int) _vIndices[i], _iRowIdx);

        info.vPos += (mul(float4(_vPos, 1.f), matBone) * _vWeight[i]).xyz;
    }

    _vPos = info.vPos;
}

#endif