#ifndef _PHYSX_DEBUG
#define _PHYSX_DEBUG

#include "value.fx"

struct VS_IN
{
    float3 vPosition : POSITION;
    float4 vColor : COLOR;
    
};

struct VS_OUT
{
    float4 vPosition : SV_Position;
    float4 vColor : COLOR;
};

VS_OUT VS_PhysxDebug(VS_IN _in)
{
    VS_OUT output = (VS_OUT) 0.f;
    
    output.vColor = _in.vColor;
    output.vPosition = mul(float4(_in.vPosition, 1.f), g_matVP);

    return output;
}


float4 PS_PhysxDebug(VS_OUT _in) : SV_Target
{
    return _in.vColor;
}

#endif