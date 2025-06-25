#ifndef _LIGHTFUNC
#define _LIGHTFUNC

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
        
        _Diffuse = Diffuse;
        _Specular = Specular;
    }
    else if (_Module.Type == POINT)
    {
        
    }
    else if (_Module.Type == SPOT)
    {
        
    }

}

#endif