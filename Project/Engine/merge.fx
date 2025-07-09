#ifndef _MERGE
#define _MERGE

#include "value.fx"

#define MERGE_MODE g_int_0

#define ALBEDO_TEX      g_tex_0
#define NORMAL_TEX      g_tex_1
#define POSITION_TEX    g_tex_2
#define EFFECT_TEX      g_tex_3

#define DIFFUSE_TEX     g_tex_4
#define SPECULAR_TEX    g_tex_5

#define EMISSIVE_TEX    g_tex_6

struct VS_IN
{
    float3 Pos : POSITION;
    float2 UV : TEXCOORD0;
};

struct VS_OUT
{
    float4 Pos : SV_Position;
    float2 UV : TEXCOORD0;
};

VS_OUT VS_Merge(VS_IN _in)
{
    VS_OUT output;
    output.Pos = float4(_in.Pos * 2.f, 1.f); // 화면 전체 채우기
    output.UV = _in.UV;
    return output;
}

float4 PS_Merge(VS_OUT _in) : SV_Target
{
    float4 vColor = (float4) 0.f;
    
    if (MERGE_MODE == -1)
    {
        // 통합 출력
        float4 vAlbedo = ALBEDO_TEX.Sample(g_sam_0, _in.UV);
        float4 vDiffuse = DIFFUSE_TEX.Sample(g_sam_0, _in.UV);
        float4 vSpecular = SPECULAR_TEX.Sample(g_sam_0, _in.UV);
        float4 vAmbient = (float4) 0.1f;
        
        float4 vEffect = EFFECT_TEX.Sample(g_sam_0, _in.UV);
        
        vColor = vAlbedo * (vDiffuse + vSpecular + vAmbient) + vEffect;

    }
    
    // ==========
    // Merge Mode
    // ==========
    
    else if (MERGE_MODE == 0)
    {
        // Albedo 출력
        vColor = ALBEDO_TEX.Sample(g_sam_0, _in.UV);
    }
    else if (MERGE_MODE == 1)
    {
        // Normal 출력
        vColor = NORMAL_TEX.Sample(g_sam_0, _in.UV);
    }
    else if (MERGE_MODE == 2)
    {
        // Position 출력
        vColor = POSITION_TEX.Sample(g_sam_0, _in.UV);
    }
    else if (MERGE_MODE == 3)
    {
        // Position 출력
        vColor = EFFECT_TEX.Sample(g_sam_0, _in.UV);
    }
    else if (MERGE_MODE == 4)
    {
        // Position 출력
        vColor = DIFFUSE_TEX.Sample(g_sam_0, _in.UV);
    }
    else if (MERGE_MODE == 5)
    {
        // Position 출력
        vColor = SPECULAR_TEX.Sample(g_sam_0, _in.UV);
    }
    else if (MERGE_MODE == 6)
    {
        // Position 출력
        vColor = EMISSIVE_TEX.Sample(g_sam_0, _in.UV);
    }

    return vColor;

}

#endif