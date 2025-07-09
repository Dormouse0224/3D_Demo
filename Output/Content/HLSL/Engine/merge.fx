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
    output.Pos = float4(_in.Pos * 2.f, 1.f); // ȭ�� ��ü ä���
    output.UV = _in.UV;
    return output;
}

float4 PS_Merge(VS_OUT _in) : SV_Target
{
    float4 vColor = (float4) 0.f;
    
    if (MERGE_MODE == -1)
    {
        // ���� ���
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
        // Albedo ���
        vColor = ALBEDO_TEX.Sample(g_sam_0, _in.UV);
    }
    else if (MERGE_MODE == 1)
    {
        // Normal ���
        vColor = NORMAL_TEX.Sample(g_sam_0, _in.UV);
    }
    else if (MERGE_MODE == 2)
    {
        // Position ���
        vColor = POSITION_TEX.Sample(g_sam_0, _in.UV);
    }
    else if (MERGE_MODE == 3)
    {
        // Position ���
        vColor = EFFECT_TEX.Sample(g_sam_0, _in.UV);
    }
    else if (MERGE_MODE == 4)
    {
        // Position ���
        vColor = DIFFUSE_TEX.Sample(g_sam_0, _in.UV);
    }
    else if (MERGE_MODE == 5)
    {
        // Position ���
        vColor = SPECULAR_TEX.Sample(g_sam_0, _in.UV);
    }
    else if (MERGE_MODE == 6)
    {
        // Position ���
        vColor = EMISSIVE_TEX.Sample(g_sam_0, _in.UV);
    }

    return vColor;

}

#endif