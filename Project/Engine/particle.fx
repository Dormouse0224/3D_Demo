#ifndef _PARTICLE
#define _PARTICLE

#include "value.fx"

#define OBJECT_POS g_vec4_0

StructuredBuffer<tParticle> m_Buffer : register(t20);
StructuredBuffer<tParticleModule> m_Module : register(t21);

struct VS_IN
{
    uint InstID : SV_InstanceID;
};


struct VS_OUT
{
    float InstID : FOG;
};

VS_OUT VS_Particle(VS_IN _in)
{
    VS_OUT output = (VS_OUT) 0.f;
    output.InstID = _in.InstID;
    return output;
}

struct GS_OUT
{
    float4 vColor : COLOR;
    float4 vPosition : SV_Position;
    float2 vUV : TEXCOORD;
};

// 1. 빌보드 구현하기
// 2. 비활성화 된 파티클을 건너뛰기
[maxvertexcount(6)]
void GS_Particle(point VS_OUT _in[1], inout TriangleStream<GS_OUT> _Stream)
{
    uint InstID = (uint) _in[0].InstID;
    if (false == m_Buffer[InstID].Active)
        return;
        
    GS_OUT output[4] = { (GS_OUT) 0.f, (GS_OUT) 0.f, (GS_OUT) 0.f, (GS_OUT) 0.f };
    
    float3 vWorldPos = m_Buffer[InstID].LocalPos;
    
    // Local 공간계일 경우에만
    if (0 == m_Module[0].SpaceType)
    {
        vWorldPos += m_Module[0].ObjectWorldPos;
    }
       
    float3 vViewPos = mul(float4(vWorldPos, 1.f), g_matView).xyz;
    
    float3 vScale = m_Buffer[InstID].WorldScale.xyz;
    
    // Viewspace 에서 파티클 위치(point mesh)를 rect mesh로 확장
    // 빌보드 효과를 구현하기 위해 View 변환 적용 후 정점 생성. (빌보드 미적용 시 World 변환 이후 정점을 생성하면 됨) - Viewspace Scale
    output[0].vPosition.xyz = float3(-vScale.x / 2.f, vScale.y / 2.f, 0);
    output[1].vPosition.xyz = float3(vScale.x / 2.f, vScale.y / 2.f, 0);
    output[2].vPosition.xyz = float3(vScale.x / 2.f, -vScale.y / 2.f, 0);
    output[3].vPosition.xyz = float3(-vScale.x / 2.f, -vScale.y / 2.f, 0);
    
    // 진행방향으로 정렬 (빌보드 무효화됨) - Viewspace Rotation
    if (m_Module[0].VelocityAllignment != 0)
    {
        float3 vU = normalize(mul(float4(m_Buffer[InstID].Velocity.xyz, 0), g_matView).xyz);
        float3 vR = normalize(cross(vU, float3(0, 0, 1)));
        float3 vF = normalize(cross(vR, vU));
        
        float3x3 ViewRotMat = { vR, vU, vF };

        for (int i = 0; i < 4; ++i)
        {
            output[i].vPosition.xyz = mul(output[i].vPosition.xyz, ViewRotMat);
        }
    }
    
    // - Viewspace Transform
    for (int i = 0; i < 4; ++i)
    {
        output[i].vPosition.xyz += vViewPos;
    }
    
    
        for (int i = 0; i < 4; ++i)
        {
            output[i].vPosition = mul(float4(output[i].vPosition.xyz, 1.f), g_matProj);
        }
    
    output[0].vUV = float2(0.f, 0.f);
    output[1].vUV = float2(1.f, 0.f);
    output[2].vUV = float2(1.f, 1.f);
    output[3].vUV = float2(0.f, 1.f);
    
    float4 Color = (m_Module[0].EndColor - m_Module[0].StartColor) * (m_Buffer[InstID].Age / m_Buffer[InstID].Life) + m_Module[0].StartColor;
    output[0].vColor = Color;
    output[1].vColor = Color;
    output[2].vColor = Color;
    output[3].vColor = Color;
        
    _Stream.Append(output[0]);
    _Stream.Append(output[1]);
    _Stream.Append(output[2]);
    _Stream.RestartStrip();

    _Stream.Append(output[0]);
    _Stream.Append(output[2]);
    _Stream.Append(output[3]);
    _Stream.RestartStrip();
    
}

float4 PS_Particle(GS_OUT _in) : SV_Target
{
    float4 vColor = g_tex_0.Sample(g_sam_0, _in.vUV);
    vColor *= _in.vColor;
    return vColor;
}

#endif