#ifndef _DEBUG_SHAPE
#define _DEBUG_SHAPE

#include "value.fx"

struct VS_IN
{
    float3 vPos : POSITION;
    float2 vUV : TEXCOORD;
};

struct VS_OUT
{
    float4 vPosition : SV_Position;
    float2 vUV : TEXCOORD;
};

VS_OUT VS_DebugShape(VS_IN _in)
{
    VS_OUT output = (VS_OUT) 0.f;
    
    output.vPosition = mul(float4(_in.vPos, 1.f), g_matWVP);
    output.vUV = _in.vUV;
    
    return output;
}

float4 PS_DebugShape(VS_OUT _in) : SV_Target
{
    return g_vec4_0;
}


// ======================
// DebugShapeSphereShader
// ======================
struct VS_IN_SPHERE
{
    float3 vPos : POSITION;
    float2 vUV : TEXCOORD;
    float3 vNormal : NORMAL;
};

struct VS_OUT_SPHERE
{
    float4 vPosition : SV_Position;
    float2 vUV : TEXCOORD;
    
    float3 vViewPos : POSITION;
    float3 vViewNormal : NORMAL;
};

VS_OUT_SPHERE VS_DebugShapeSphere(VS_IN_SPHERE _in)
{
    VS_OUT_SPHERE output = (VS_OUT_SPHERE) 0.f;
    
    output.vPosition = mul(float4(_in.vPos, 1.f), g_matWVP);
    output.vUV = _in.vUV;
    
    output.vViewPos = mul(float4(_in.vPos, 1.f), g_matWV);
    output.vViewNormal = normalize(mul(float4(_in.vNormal, 0.f), g_matWV));
    
    return output;
}

float4 PS_DebugShapeSphere(VS_OUT_SPHERE _in) : SV_Target
{
    float3 vEye = normalize(_in.vViewPos);
    
    // 카메라를 마주보는 정도
    float Alpha = saturate(1.f - abs(dot(-vEye, _in.vViewNormal)));
    Alpha = pow(Alpha, 5);
    
    float4 vColor = g_vec4_0;
    vColor.a = Alpha;
    
    return vColor;
}





#endif