#ifndef _UI
#define _UI

#include "value.fx"

#define TEXTURE     g_tex_0
#define ScaleX      g_float_1
#define ScaleY      g_float_2
#define NDC         g_vec2_0


// Vertex Shader
struct VS_IN
{
    float3 vPos : POSITION; // Semantic, InputLayout
};
struct VS_OUT
{
    float4 vPosition : POSITION;
};

VS_OUT VS_UI(VS_IN _in)
{
    // 정점 셰이더가 전달하는 정점은 사용하지 않으므로 최소 정보만 전달한다.
    VS_OUT output = (VS_OUT) 0.f;
    
    output.vPosition = float4(_in.vPos.xyz, 1.f);
    return output;
}

struct GS_OUT
{
    float4 vPosition : SV_Position;
    float2 vUV : TEXCOORD;
};

[maxvertexcount(6)]
void GS_UI(point VS_OUT _in[1], inout TriangleStream<GS_OUT> _Stream)
{
    // 정점 셰이더에서 받은 정점을 사용하지 않고, 바인딩된 NDC 좌표와 스케일값을 이용해 지오메트리 셰이더에서 사각형 메쉬를 직접 생성한다.
    GS_OUT output[4] = { (GS_OUT) 0.f, (GS_OUT) 0.f, (GS_OUT) 0.f, (GS_OUT) 0.f };
    
    // 바인딩된 UI의 NDC 좌표에 스케일을 NDC 기준으로 변환한 후, 사각형 메쉬로 확장
    // 이 좌표는 WVP 변환 후 원근분할까지 적용된 좌표이므로, near 평면에 위치하도록 Z 값을 0으로 하고 래스터라이저에서 원근분할이 일어나지 않도록 W 값을 1로 설정한다.
    output[0].vPosition = float4(NDC.x - ScaleX / g_Resolution.x, NDC.y + ScaleY / g_Resolution.y, 0, 1.f);
    output[1].vPosition = float4(NDC.x + ScaleX / g_Resolution.x, NDC.y + ScaleY / g_Resolution.y, 0, 1.f);
    output[2].vPosition = float4(NDC.x + ScaleX / g_Resolution.x, NDC.y - ScaleY / g_Resolution.y, 0, 1.f);
    output[3].vPosition = float4(NDC.x - ScaleX / g_Resolution.x, NDC.y - ScaleY / g_Resolution.y, 0, 1.f);
    
    
    output[0].vUV = float2(0.f, 0.f);
    output[1].vUV = float2(1.f, 0.f);
    output[2].vUV = float2(1.f, 1.f);
    output[3].vUV = float2(0.f, 1.f);
        
    _Stream.Append(output[0]);
    _Stream.Append(output[1]);
    _Stream.Append(output[2]);
    _Stream.RestartStrip();

    _Stream.Append(output[0]);
    _Stream.Append(output[2]);
    _Stream.Append(output[3]);
    _Stream.RestartStrip();
    
}

float4 PS_UI(GS_OUT _in) : SV_Target
{
    float4 vSampleColor = TEXTURE.Sample(g_sam_1, _in.vUV);
    vSampleColor.a = vSampleColor.a * g_float_0; // 알파값 보정치 (그라데이션용)
    
    return vSampleColor;
}

#endif