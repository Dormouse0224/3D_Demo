#ifndef _SHADOW
#define _SHADOW

#include "value.fx"
#include "func.fx"

#define LIGHT_TYPE g_int_0
#define CASCADE g_int_1

struct VS_IN
{
    float3 vPos : POSITION;
    
    // 3D Animatino 관련 정보
    float4 vWeights : BLENDWEIGHT;
    float4 vIndices : BLENDINDICES;
};

struct VS_OUT
{
    float4 vPosition : SV_Position;
    //float3 vProjPos : POSITION;
};

VS_OUT VS_Shadow(VS_IN _in)
{
    VS_OUT output = (VS_OUT) 0.f;
        
    if (g_iAnim)
    {
        Skinning_LocalPos(_in.vPos, _in.vWeights, _in.vIndices, 0);
    }
    
    if (LIGHT_TYPE == 0) // Directional
    {
        if (CASCADE == 0) // Near
            output.vPosition = mul(float4(_in.vPos, 1.f), mul(g_matWorld, g_matShadowVP_0));
        else if (CASCADE == 1) // Mid
            output.vPosition = mul(float4(_in.vPos, 1.f), mul(g_matWorld, g_matShadowVP_1));
        else if (CASCADE == 2) // Far
            output.vPosition = mul(float4(_in.vPos, 1.f), mul(g_matWorld, g_matShadowVP_2));
    }
    else if (LIGHT_TYPE == 1) // Point
    {
        if (CASCADE == 0) // Right
            output.vPosition = mul(float4(_in.vPos, 1.f), mul(g_matWorld, g_matShadowVP_0));
        else if (CASCADE == 1) // Left
            output.vPosition = mul(float4(_in.vPos, 1.f), mul(g_matWorld, g_matShadowVP_1));
        else if (CASCADE == 2) // Top
            output.vPosition = mul(float4(_in.vPos, 1.f), mul(g_matWorld, g_matShadowVP_2));
        else if (CASCADE == 3) // Bottom
            output.vPosition = mul(float4(_in.vPos, 1.f), mul(g_matWorld, g_matShadowVP_3));
        else if (CASCADE == 4) // Front
            output.vPosition = mul(float4(_in.vPos, 1.f), mul(g_matWorld, g_matShadowVP_4));
        else if (CASCADE == 5) // Back
            output.vPosition = mul(float4(_in.vPos, 1.f), mul(g_matWorld, g_matShadowVP_5));
    }
    else if (LIGHT_TYPE == 2) // Spot
    {
        output.vPosition = mul(float4(_in.vPos, 1.f), mul(g_matWorld, g_matShadowVP_0));
    }
    
    // output.vPosition == (Px * Vz, Py * Vz, Pz * Vz, Vz)
    //output.vProjPos = output.vPosition.xyz / output.vPosition.w;
        
    return output;
}

struct GS_OUT
{
    float4 vPosition : SV_POSITION;
    //float3 vProjPos : POSITION;
    uint RTIndex : SV_RenderTargetArrayIndex;
};

// 최종 출력 정점수 3개, 입력 정점은 삼각형이므로 3개, 출력 토폴로지는 삼각형으로 지오메트리 셰이더를 구성.
// 입력받은 삼각형에 추가로 렌더 타겟 슬라이스의 인덱스 정보를 더해서 출력한다.
[maxvertexcount(3)]
void GS_Shadow(triangle VS_OUT input[3], inout TriangleStream<GS_OUT> _Stream)
{
    for (int i = 0; i < 3; ++i)
    {
        GS_OUT output;
        output.vPosition = input[i].vPosition;
        //output.vProjPos = input[i].vProjPos;
        if (LIGHT_TYPE == 0)
            output.RTIndex = CASCADE; // 배열 렌더 타겟 슬라이스 지정
        else if (LIGHT_TYPE == 1)
            output.RTIndex = CASCADE;
        else if(LIGHT_TYPE == 2)
            output.RTIndex = 0;
        _Stream.Append(output);
    }
}

float PS_Shadow(GS_OUT _in) : SV_Target
{
    return _in.vPosition.z;
}


#endif