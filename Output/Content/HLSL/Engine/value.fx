#ifndef _VALUE
#define _VALUE

#include "struct.fx"

#define PI 3.1415926535f

// 상수 레지스터
// HLSL 5.0 : 4096
cbuffer TRANSFORM : register(b0)
{
    row_major matrix g_matWorld;
    row_major matrix g_matView;
    row_major matrix g_matProj;
    
    row_major matrix g_matWV;
    row_major matrix g_matVP;
    row_major matrix g_matWVP;
}

cbuffer MATERIAL : register(b1)
{
    int g_int_0;
    int g_int_1;
    int g_int_2;
    int g_int_3;
                     
    float g_float_0;
    float g_float_1;
    float g_float_2;
    float g_float_3;
                     
    float2 g_vec2_0;
    float2 g_vec2_1;
    float2 g_vec2_2;
    float2 g_vec2_3;
                     
    float4 g_vec4_0;
    float4 g_vec4_1;
    float4 g_vec4_2;
    float4 g_vec4_3;
    
    row_major Matrix g_mat_0;
    row_major Matrix g_mat_1;
    row_major Matrix g_mat_2;
    row_major Matrix g_mat_3;
    
    int g_btex_0;
    int g_btex_1;
    int g_btex_2;
    int g_btex_3;
    int g_btex_4;
    int g_btex_5;
    
    int2 g_padding;
}

cbuffer GLOBAL : register(b2)
{
    float2 g_Resolution;
    float g_DT;
    float g_EngineDT;
    float g_Time;
    float g_EngineTime;
    int g_Light2DCount;
    int g_Light3DCount;
}




// 샘플러
SamplerState g_sam_0 : register(s0);
SamplerState g_sam_1 : register(s1);


// 텍스쳐 레지스터
Texture2D g_tex_0 : register(t0);
Texture2D g_tex_1 : register(t1);
Texture2D g_tex_2 : register(t2);
Texture2D g_tex_3 : register(t3);
Texture2D g_tex_4 : register(t4);
Texture2D g_tex_5 : register(t5);

// 노멀맵 텍스쳐 레지스터
Texture2D g_Ntex_0 : register(t6);
Texture2D g_Ntex_1 : register(t7);
Texture2D g_Ntex_2 : register(t8);
Texture2D g_Ntex_3 : register(t9);
Texture2D g_Ntex_4 : register(t10);
Texture2D g_Ntex_5 : register(t11);

// 광원 구조화 버퍼
StructuredBuffer<tLightModule> g_LightBuffer : register(t12);









#endif