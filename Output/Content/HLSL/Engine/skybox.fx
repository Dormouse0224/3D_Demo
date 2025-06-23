#ifndef _SKYBOX
#define _SKYBOX

#include "value.fx"

#define SKYTEX g_cubetex_0

struct VS_IN
{
    float3 vPos : POSITION;
    //float2 vUV : TEXCOORD;
};

struct VS_OUT
{
    float4 vPos : SV_Position;
    //float2 vUV : TEXCOORD;
    float3 vLocalDir : POSITION;
};

VS_OUT VS_SkyBox(VS_IN _In)
{
    VS_OUT output = (VS_OUT) 0.f;
    
    // 방향 정보로 투영
    output.vPos = mul(float4(_In.vPos, 0.f), g_matVP);
    // 래스터라이저에서 원근분할 시 z 값이 1로 고정되도록 z 값에 w 값을 넣음
    output.vPos.z = output.vPos.w;
    
    //output.vUV = _In.vUV;
    
    // 큐브 텍스쳐 샘플링용 방향 벡터
    output.vLocalDir = normalize(_In.vPos);

    return output;
}

float4 PS_SkyBox(VS_OUT _In) : SV_Target
{
    float4 color = float4(1.f, 0.f, 1.f, 1.f);
    
    float3 vDirUV = normalize(_In.vLocalDir);
    color = SKYTEX.Sample(g_sam_0, vDirUV);
    
    return color;
}

#endif