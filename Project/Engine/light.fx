#ifndef _LIGHT
#define _LIGHT

#include "value.fx"
#include "func.fx"

#define NORMAL_TEX g_tex_0
#define POSITION_TEX g_tex_1

#define DIRECTIONAL 0
#define POINT 1
#define SPOT 2

#define LIGHT_INDEX g_int_0

struct VS_IN
{
    float3 vPos : POSITION;
    float2 vUV : TEXCOORD0;
};

struct VS_OUT
{
    float4 vPos : SV_Position;
    float2 vUV : TEXCOORD0;
};

VS_OUT VS_Light(VS_IN _In)
{
    VS_OUT _Out;
    if (g_LightBuffer[LIGHT_INDEX].Type == DIRECTIONAL)
        _Out.vPos = float4(_In.vPos * 2.f, 1.f);
    else if (g_LightBuffer[LIGHT_INDEX].Type == POINT)
        _Out.vPos = mul(float4(_In.vPos, 1.f), g_matWVP);
    _Out.vUV = _In.vUV;
    
    return _Out;
}

struct PS_OUT
{
    float4 vDiffuse : SV_Target0;
    float4 vSpecular : SV_Target1;
};

// Lighting
// 기본 오브젝트 색상 + 조명 색상
PS_OUT PS_Light(VS_OUT _In)
{
    PS_OUT output;
    output.vDiffuse = float4(0.f, 0.f, 0.f, 1.f);
    output.vSpecular = float4(0.f, 0.f, 0.f, 1.f);
    
    float2 SamplePos = float2(0.f, 0.f);
    if (g_LightBuffer[LIGHT_INDEX].Type == DIRECTIONAL)
    {
        SamplePos = _In.vUV;
    }
    else if (g_LightBuffer[LIGHT_INDEX].Type == POINT || g_LightBuffer[LIGHT_INDEX].Type == SPOT)
    {
        SamplePos = _In.vPos.xy / g_Resolution;
    }
    
    float4 ViewPos = POSITION_TEX.Sample(g_sam_0, SamplePos);
    float4 ViewNorm = NORMAL_TEX.Sample(g_sam_0, SamplePos);
    
    if (ViewPos.x == 0 && ViewPos.y == 0 && ViewPos.z == 0 && ViewPos.w == 0)
        discard;
    
    
    CalcLight(ViewPos.xyz, ViewNorm.xyz, g_LightBuffer[LIGHT_INDEX], 2.f, output.vDiffuse.xyz, output.vSpecular.xyz);
    
    
    return output;
}



#endif