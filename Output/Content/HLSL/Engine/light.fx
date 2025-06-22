#ifndef _LIGHT
#define _LIGHT

#include "value.fx"

#define TEX_Color g_tex_0
#define TEX_Normal g_tex_1
#define TEX_ViewPos g_tex_2

#define DIRECTIONAL 0
#define POINT 1
#define SPOT 2

struct VS_IN
{
    float3 Pos : POSITION;
    float2 UV : TEXCOORD0;
    
    float3 Tangent : TANGENT;
    float3 Normal : NORMAL;
    float3 Binormal : BINORMAL;
};

struct VS_OUT
{
    float4 Pos : SV_Position;
    float2 UV : TEXCOORD0;
    
    float3 Tangent : TANGENT;
    float3 Normal : NORMAL;
    float3 Binormal : BINORMAL;
};

VS_OUT VS_Light(VS_IN _in)
{
    VS_OUT _Out;
    _Out.Pos = float4(_in.Pos * 2.f, 1.f);
    _Out.UV = _in.UV;
    
    _Out.Tangent = mul(float4(_in.Tangent, 0), g_matWV);
    _Out.Normal = mul(float4(_in.Normal, 0), g_matWV);
    _Out.Binormal = mul(float4(_in.Binormal, 0), g_matWV);
    
    return _Out;
}

// Lighting
// 기본 오브젝트 색상 + 조명 색상
float4 PS_Light(VS_OUT _in) : SV_Target
{
    float4 _Out;
    
    float3x3 TBN = { float3(-1, 0, 0), float3(0, 1, 0), float3(0, 0, -1) };
    
    for (int i = 0; i < g_Light2DCount; ++i)
    {
        if (g_LightBuffer[i].Type == DIRECTIONAL)
        {
            // Diffuse
            float3 vLightDir = normalize(g_LightBuffer[i].Dir);
            float3x3 matTBN = { _in.Tangent, _in.Binormal, _in.Normal };
            float3 vNormal = normalize(mul(TEX_Normal.Sample(g_sam_0, _in.UV).xyz * 2.f - 1.f, matTBN));
            float3 diffuse = max(dot(-vLightDir, vNormal), 0.f) * TEX_Color.Sample(g_sam_0, _in.UV).xyz;
            
            // Specular
            float3 vReflect = vLightDir + 2 * dot(-vLightDir, vNormal) * vNormal;
            float3 vCamDir = normalize(-_in.Pos);
            float specPow = 32.f;
            float3 specular = pow(max(dot(vReflect, vCamDir), 0.f), specPow) * float3(1.f, 1.f, 1.f);
            
            // Ambient
            float3 ambient = 0.1f * TEX_Color.Sample(g_sam_0, _in.UV).xyz;
            
            _Out = float4(diffuse + specular + ambient, 1.f);
        }
        else if (g_LightBuffer[i].Type == POINT)
        {
            
        }
        else if (g_LightBuffer[i].Type == SPOT)
        {
            
        }
    }
    
    
    return _Out;
}



#endif