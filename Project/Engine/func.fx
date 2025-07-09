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