#ifndef _FLIPBOOK
#define _FLIPBOOK

#include "value.fx"

#define IS_ATLAS_TEX    g_int_0
#define IS_SINGLE_TEX   g_int_1

#define TEXTURE         g_tex_0
#define NORM_TEXTURE    g_Ntex_0

#define LeftTop         g_vec2_0
#define Slice           g_vec2_1

struct VS_IN
{
    float3 vPos : POSITION; // Semantic, InputLayout   
    float2 vUV : TEXCOORD;
    
    float3 Tangent : TANGENT;
    float3 Normal : NORMAL;
    float3 Binormal : BINORMAL;
};

struct VS_OUT
{
    float4 vPosition : SV_Position;
    float2 vUV : TEXCOORD;
    float4 vViewPos : POSITION;
    
    float3 Tangent : TANGENT;
    float3 Normal : NORMAL;
    float3 Binormal : BINORMAL;
};

VS_OUT VS_Flipbook(VS_IN _in)
{
    VS_OUT output = (VS_OUT) 0.f;
                
    output.vPosition = mul(float4(_in.vPos, 1.f), g_matWVP);
    output.vUV = _in.vUV;
    output.vViewPos = mul(float4(_in.vPos, 1.f), g_matWV);
    
    output.Tangent = normalize(mul(float4(_in.Tangent, 0), g_matWV));
    output.Normal = normalize(mul(float4(_in.Normal, 0), g_matWV));
    output.Binormal = normalize(mul(float4(_in.Binormal, 0), g_matWV));
    
    return output;
}

struct PS_OUT
{
    float4 RT_Color : SV_Target0;
    float4 RT_Normal : SV_Target1;
    float4 RT_ViewPos : SV_Target2;
};

PS_OUT PS_Flipbook(VS_OUT _in)
{
    PS_OUT _Out;
    
    _Out.RT_Color = float4(1.f, 0.f, 1.f, 1.f); // 기본 색상 마젠타로 초기화
    _Out.RT_Normal = float4(0.f, 0.f, 0.f, 0.f);
    _Out.RT_ViewPos = float4(0.f, 0.f, 0.f, 1.f);
    
    if (1 == IS_ATLAS_TEX)
    {
        float2 vUV = LeftTop + (Slice * _in.vUV);
        
        if (vUV.x < LeftTop.x || (LeftTop.x + Slice.x) < vUV.x
            || vUV.y < LeftTop.y || (LeftTop.y + Slice.y) < vUV.y)
        {
            discard;
        }
        
        _Out.RT_Color = TEXTURE.Sample(g_sam_1, vUV);
        float3x3 matTBN = { _in.Tangent, _in.Binormal, _in.Normal };
        float3 vNormal = normalize(mul(NORM_TEXTURE.Sample(g_sam_0, vUV).xyz * 2.f - 1.f, matTBN));
        _Out.RT_Normal = float4(vNormal, 1.f);
        _Out.RT_ViewPos = _in.vViewPos;

    }
    else if (1 == IS_SINGLE_TEX)
    {
        float2 vUV = _in.vUV;
        _Out.RT_Color = TEXTURE.Sample(g_sam_1, vUV);
        float3x3 matTBN = { _in.Tangent, _in.Binormal, _in.Normal };
        float3 vNormal = normalize(mul(NORM_TEXTURE.Sample(g_sam_0, vUV).xyz * 2.f - 1.f, matTBN));
        _Out.RT_Normal = float4(vNormal, 1.f);
        _Out.RT_ViewPos = _in.vViewPos;
    }
    
    if (0.f == _Out.RT_Color.a)
        discard;
    
    return _Out;
}


#endif