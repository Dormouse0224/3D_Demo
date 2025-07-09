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
    
    row_major matrix g_matInvWorld;
    row_major matrix g_matInvView;
    row_major matrix g_matInvProj;
    
    row_major matrix g_matWV;
    row_major matrix g_matVP;
    row_major matrix g_matWVP;
}

cbuffer MATERIAL : register(b1)
{
    // 재질 계수
    float4 g_vDiff;
    float4 g_vSpec;
    float4 g_vAmb;
    float4 g_vEmv;
    
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
    int g_btex_6;
    int g_btex_7;
    
    int g_btexCube_0;
    int g_btexCube_1;
    int g_btexCube_2;
    int g_btexCube_3;
    
    int g_btexArr_0;
    int g_btexArr_1;
    int g_btexArr_2;
    int g_btexArr_3;
    
    // 3D Animation 정보
    int g_iAnim;
    int g_iBoneCount;
    
    int2 padding;
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
Texture2D g_tex_6 : register(t6);
Texture2D g_tex_7 : register(t7);

TextureCube g_cubetex_0 : register(t8);
TextureCube g_cubetex_1 : register(t9);
TextureCube g_cubetex_2 : register(t10);
TextureCube g_cubetex_3 : register(t11);

Texture2DArray g_texarr_0 : register(t12);
Texture2DArray g_texarr_1 : register(t13);
Texture2DArray g_texarr_2 : register(t14);
Texture2DArray g_texarr_3 : register(t15);

// 광원 구조화 버퍼
StructuredBuffer<tLightModule> g_LightBuffer : register(t16);

// Animation3D Bone Matrix Buffer
StructuredBuffer<Matrix> g_arrBoneMat : register(t17);








#endif