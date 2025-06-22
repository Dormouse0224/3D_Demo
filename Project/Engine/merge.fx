#ifndef _MERGE
#define _MERGE

#include "value.fx"

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
    float4 vColor = g_tex_0.Sample(g_sam_0, _in.UV);

    return vColor;

}

#endif