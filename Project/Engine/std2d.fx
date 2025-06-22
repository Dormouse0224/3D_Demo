#ifndef _STD2D
#define _STD2D

#include "value.fx"

#define TEXTURE         g_tex_0
#define NORM_TEXTURE    g_Ntex_0

// Vertex Shader
struct VS_IN
{
    float3 vPos : POSITION; // Semantic, InputLayout
    float4 vColor : COLOR;
    float2 vUV : TEXCOORD;
    
    float3 Tangent : TANGENT;
    float3 Normal : NORMAL;
    float3 Binormal : BINORMAL;
};

struct VS_OUT
{
    // System Value Semantic
    // SV_Position : 레스터라이져에 NDC 좌표를 전달할 때 사용하는 Semantic
    float4 vPosition : SV_Position;
    float4 vColor : COLOR;
    float2 vUV : TEXCOORD;
    float4 vViewPos : POSITION;
    
    float3 Tangent : TANGENT;
    float3 Normal : NORMAL;
    float3 Binormal : BINORMAL;
};

VS_OUT VS_Std2D(VS_IN _in)
{
    VS_OUT output = (VS_OUT) 0.f;
                
    output.vPosition = mul(float4(_in.vPos, 1.f), g_matWVP);
    output.vColor = _in.vColor;
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

PS_OUT PS_Std2D(VS_OUT _in)
{
    PS_OUT _Out;
    
    _Out.RT_Color = float4(1.f, 0.f, 1.f, 1.f);
    _Out.RT_Normal = float4(1.f, 1.f, 1.f, 0.f);
    _Out.RT_ViewPos = float4(0.f, 0.f, 0.f, 1.f);
    
    _Out.RT_Color = TEXTURE.Sample(g_sam_1, _in.vUV);
    float3x3 matTBN = { _in.Tangent, _in.Binormal, _in.Normal };
    float3 vNormal = normalize(mul(NORM_TEXTURE.Sample(g_sam_0, _in.vUV).xyz * 2.f - 1.f, matTBN));
    _Out.RT_Normal = float4(vNormal, 1.f);
    _Out.RT_ViewPos = _in.vViewPos;
          
    if (_Out.RT_Color.a == 0.f)
        discard;
    
    // 그레이 스케일
    if (g_int_0)
    {
        _Out.RT_Color.rgb = (_Out.RT_Color.r + _Out.RT_Color.g + _Out.RT_Color.b) / 3.f;
    }
    
    return _Out;
}


PS_OUT PS_Std2D_AlphaBlend(VS_OUT _in)
{
    PS_OUT _Out;
    
    _Out.RT_Color = float4(1.f, 0.f, 1.f, 1.f);
    _Out.RT_Normal = float4(0.f, 0.f, 0.f, 0.f);
    _Out.RT_ViewPos = float4(0.f, 0.f, 0.f, 1.f);
    
    _Out.RT_Color = TEXTURE.Sample(g_sam_1, _in.vUV);
    _Out.RT_Normal = NORM_TEXTURE.Sample(g_sam_1, _in.vUV);
    _Out.RT_ViewPos = _in.vViewPos;
    
    _Out.RT_Color.a = _Out.RT_Color.a * g_float_0; // 알파값 보정치 (그라데이션용)
    
    return _Out;
}

#endif