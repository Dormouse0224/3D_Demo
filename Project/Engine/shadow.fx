#ifndef _SHADOW
#define _SHADOW

#include "value.fx"

#define CASCADE g_int_0

struct VS_IN
{
    float3 vPos : POSITION;
    
    // 3D Animatino ���� ����
    float4 vWeights : BLENDWEIGHT;
    float4 vIndices : BLENDINDICES;
};

struct VS_OUT
{
    float4 vPosition : SV_Position;
    float3 vProjPos : POSITION;
};

VS_OUT VS_Shadow(VS_IN _in)
{
    VS_OUT output = (VS_OUT) 0.f;
        
    //if (g_iAnim)
    //{
    //    Skinning_LocalPos(_in.vPos, _in.vWeights, _in.vIndices, 0);
    //}
    
    output.vPosition = mul(float4(_in.vPos, 1.f), g_matWVP);
    
    // output.vPosition == (Px * Vz, Py * Vz, Pz * Vz, Vz)
    output.vProjPos = output.vPosition.xyz / output.vPosition.w;
        
    return output;
}

struct GS_OUT
{
    float4 vPosition : SV_POSITION;
    float3 vProjPos : POSITION;
    uint RTIndex : SV_RenderTargetArrayIndex;
};

// ���� ��� ������ 3��, �Է� ������ �ﰢ���̹Ƿ� 3��, ��� ���������� �ﰢ������ ������Ʈ�� ���̴��� ����.
// �Է¹��� �ﰢ���� �߰��� ���� Ÿ�� �����̽��� �ε��� ������ ���ؼ� ����Ѵ�.
[maxvertexcount(3)]
void GS_Shadow(triangle VS_OUT input[3], inout TriangleStream<GS_OUT> _Stream)
{
    for (int i = 0; i < 3; ++i)
    {
        GS_OUT output;
        output.vPosition = input[i].vPosition;
        output.vProjPos = input[i].vProjPos;
        output.RTIndex = CASCADE; // �迭 ���� Ÿ�� �����̽� ����
        _Stream.Append(output);
    }
}

float PS_Shadow(GS_OUT _in) : SV_Target
{
    return _in.vProjPos.z;
}


#endif