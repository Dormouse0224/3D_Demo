#ifndef _LIGHT
#define _LIGHT

#include "value.fx"
#include "lightfunc.fx"

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
    _Out.vPos = float4(_In.vPos * 2.f, 1.f);
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
    
    float4 ViewPos = POSITION_TEX.Sample(g_sam_0, _In.vUV);
    float4 ViewNorm = NORMAL_TEX.Sample(g_sam_0, _In.vUV);
    if (ViewPos.x == 0 && ViewPos.y == 0 && ViewPos.z == 0 && ViewPos.w == 0)
        discard;
    
    
    CalcLight(ViewPos.xyz, ViewNorm.xyz, g_LightBuffer[LIGHT_INDEX], 2.f, output.vDiffuse.xyz, output.vSpecular.xyz);
    
    //output.vDiffuse = ViewPos;
    //output.vSpecular = ViewNorm;
    
    return output;
}



#endif