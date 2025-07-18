#ifndef _LIGHT
#define _LIGHT

#include "value.fx"
#include "func.fx"

#define NORMAL_TEX g_tex_0
#define POSITION_TEX g_tex_1

#define DIRECTIONAL 0
#define POINT 1
#define SPOT 2

#define SHADOWMAP_D g_texarr_0
#define SHADOWMAP_P g_cubetex_0
#define SHADOWMAP_S g_tex_2

#define LIGHT_INDEX g_int_0
#define CSM         g_int_1

struct VS_IN
{
    float3 vPos : POSITION;
    float2 vUV : TEXCOORD0;
};

struct VS_OUT
{
    float4 vPos : SV_Position;
    float2 vUV : TEXCOORD0;
};

VS_OUT VS_Light(VS_IN _In)
{
    VS_OUT _Out;
    if (g_LightBuffer[LIGHT_INDEX].Type == DIRECTIONAL)
        _Out.vPos = float4(_In.vPos * 2.f, 1.f);
    else
        _Out.vPos = mul(float4(_In.vPos, 1.f), g_matWVP);
    _Out.vUV = _In.vUV;
    
    return _Out;
}

struct PS_OUT
{
    float4 vDiffuse : SV_Target0;
    float4 vSpecular : SV_Target1;
};

// Lighting
// �⺻ ������Ʈ ���� + ���� ����
PS_OUT PS_Light(VS_OUT _In)
{
    PS_OUT output;
    output.vDiffuse = float4(0.f, 0.f, 0.f, 1.f);
    output.vSpecular = float4(0.f, 0.f, 0.f, 1.f);
    
    float2 SamplePos = float2(0.f, 0.f);
    if (g_LightBuffer[LIGHT_INDEX].Type == DIRECTIONAL)
    {
        SamplePos = _In.vUV;
    }
    else if (g_LightBuffer[LIGHT_INDEX].Type == POINT || g_LightBuffer[LIGHT_INDEX].Type == SPOT)
    {
        SamplePos = _In.vPos.xy / g_Resolution;
    }
    
    float4 ViewPos = POSITION_TEX.Sample(g_sam_0, SamplePos);
    float4 ViewNorm = NORMAL_TEX.Sample(g_sam_0, SamplePos);
    
    if (ViewPos.x == 0 && ViewPos.y == 0 && ViewPos.z == 0 && ViewPos.w == 0)
        discard;
    
    // �� ���
    CalcLight(ViewPos.xyz, ViewNorm.xyz, g_LightBuffer[LIGHT_INDEX], 2.f, output.vDiffuse.xyz, output.vSpecular.xyz);
    
    // �׸��� ���
    if (g_LightBuffer[LIGHT_INDEX].Type == DIRECTIONAL)
    {
        if (CSM == 1)
        {
            float far = 10000.f;
            if (ViewPos.z < far * 0.01f)
            {
                float4 vWorldPos = mul(ViewPos, g_matInvView);
                float4 vProjPos = mul(vWorldPos, g_matShadowVP_0);
                vProjPos = vProjPos / vProjPos.w;
            
                // �������� ������Ų NDC ��ǥ�� UV �� ��ȯ�ؼ� ���������� ��ϵ� ��ü�� ���̸� Ȯ���Ѵ�.
                float3 vShadowMapUV = float3((vProjPos.x + 1.f) * 0.5f, 1.f - (vProjPos.y + 1.f) * 0.5f, 0);
                float fDist = SHADOWMAP_D.Sample(g_sam_0, vShadowMapUV).x;
            
                // ���� �������� ��ü�� ��ϵ� ����(�þ� ����) �̳������� �׽�Ʈ�� �����Ѵ�.
                if (0.f < vShadowMapUV.x && vShadowMapUV.x < 1.f
            && 0.f < vShadowMapUV.y && vShadowMapUV.y < 1.f)
                {
                    // ������������ ��ϵ� ���̰���, ������ ���̸� ���Ѵ�.
                    // ��ϵ� ���̺��� ���� ������Ų ���̰� �� ��ٸ�, ������������ ������ �����̴� ==> �׸��ڰ� ���ܾ� �Ѵ�.
                    if (fDist + 0.0001f < vProjPos.z)
                        discard;
                }
            }
            else if (ViewPos.z < far * 0.1f)
            {
                float4 vWorldPos = mul(ViewPos, g_matInvView);
                float4 vProjPos = mul(vWorldPos, g_matShadowVP_1);
                vProjPos = vProjPos / vProjPos.w;
            
                // �������� ������Ų NDC ��ǥ�� UV �� ��ȯ�ؼ� ���������� ��ϵ� ��ü�� ���̸� Ȯ���Ѵ�.
                float3 vShadowMapUV = float3((vProjPos.x + 1.f) * 0.5f, 1.f - (vProjPos.y + 1.f) * 0.5f, 1);
                float fDist = SHADOWMAP_D.Sample(g_sam_0, vShadowMapUV).x;
            
                // ���� �������� ��ü�� ��ϵ� ����(�þ� ����) �̳������� �׽�Ʈ�� �����Ѵ�.
                if (0.f < vShadowMapUV.x && vShadowMapUV.x < 1.f
            && 0.f < vShadowMapUV.y && vShadowMapUV.y < 1.f)
                {
                    // ������������ ��ϵ� ���̰���, ������ ���̸� ���Ѵ�.
                    // ��ϵ� ���̺��� ���� ������Ų ���̰� �� ��ٸ�, ������������ ������ �����̴� ==> �׸��ڰ� ���ܾ� �Ѵ�.
                    if (fDist + 0.0001f < vProjPos.z)
                        discard;
                }
            }
            else
            {
                float4 vWorldPos = mul(ViewPos, g_matInvView);
                float4 vProjPos = mul(vWorldPos, g_matShadowVP_2);
                vProjPos = vProjPos / vProjPos.w;
            
                // �������� ������Ų NDC ��ǥ�� UV �� ��ȯ�ؼ� ���������� ��ϵ� ��ü�� ���̸� Ȯ���Ѵ�.
                float3 vShadowMapUV = float3((vProjPos.x + 1.f) * 0.5f, 1.f - (vProjPos.y + 1.f) * 0.5f, 2);
                float fDist = SHADOWMAP_D.Sample(g_sam_0, vShadowMapUV).x;
            
                // ���� �������� ��ü�� ��ϵ� ����(�þ� ����) �̳������� �׽�Ʈ�� �����Ѵ�.
                if (0.f < vShadowMapUV.x && vShadowMapUV.x < 1.f
            && 0.f < vShadowMapUV.y && vShadowMapUV.y < 1.f)
                {
                    // ������������ ��ϵ� ���̰���, ������ ���̸� ���Ѵ�.
                    // ��ϵ� ���̺��� ���� ������Ų ���̰� �� ��ٸ�, ������������ ������ �����̴� ==> �׸��ڰ� ���ܾ� �Ѵ�.
                    if (fDist + 0.0001f < vProjPos.z)
                        discard;
                }
            }
        }
        else
        {
            float4 vWorldPos = mul(ViewPos, g_matInvView);
            float4 vProjPos = mul(vWorldPos, g_matShadowVP_0);
            vProjPos = vProjPos / vProjPos.w;
            
            // �������� ������Ų NDC ��ǥ�� UV �� ��ȯ�ؼ� ���������� ��ϵ� ��ü�� ���̸� Ȯ���Ѵ�.
            float3 vShadowMapUV = float3((vProjPos.x + 1.f) * 0.5f, 1.f - (vProjPos.y + 1.f) * 0.5f, 0);
            float fDist = SHADOWMAP_D.Sample(g_sam_0, vShadowMapUV).x;
            
            // ���� �������� ��ü�� ��ϵ� ����(�þ� ����) �̳������� �׽�Ʈ�� �����Ѵ�.
            if (0.f < vShadowMapUV.x && vShadowMapUV.x < 1.f
            && 0.f < vShadowMapUV.y && vShadowMapUV.y < 1.f)
            {
                // ������������ ��ϵ� ���̰���, ������ ���̸� ���Ѵ�.
                // ��ϵ� ���̺��� ���� ������Ų ���̰� �� ��ٸ�, ������������ ������ �����̴� ==> �׸��ڰ� ���ܾ� �Ѵ�.
                if (fDist + 0.0001f < vProjPos.z)
                    discard;
            }
        }
    }
    else if (g_LightBuffer[LIGHT_INDEX].Type == POINT)
    {
        float4 vWorldPos = mul(ViewPos, g_matInvView);
        float4 vProjPos = (float4) 0.f;
        float3 LocalPos = mul(mul(ViewPos, g_matInvView), g_matInvWorld).xyz;
        float3 LocalDir = normalize(LocalPos);
        float maxVal = max(abs(LocalDir.x), max(abs(LocalDir.y), abs(LocalDir.z)));
        if (maxVal == LocalDir.x)
            vProjPos = mul(vWorldPos, g_matShadowVP_0);
        else if (maxVal == -LocalDir.x)
            vProjPos = mul(vWorldPos, g_matShadowVP_1);
        else if (maxVal == LocalDir.y)
            vProjPos = mul(vWorldPos, g_matShadowVP_2);
        else if (maxVal == -LocalDir.y)
            vProjPos = mul(vWorldPos, g_matShadowVP_3);
        else if (maxVal == LocalDir.z)
            vProjPos = mul(vWorldPos, g_matShadowVP_4);
        else if (maxVal == -LocalDir.z)
            vProjPos = mul(vWorldPos, g_matShadowVP_5);
        
        vProjPos = vProjPos / vProjPos.w;
            
        // �������� ������Ų NDC ��ǥ�� UV �� ��ȯ�ؼ� ���������� ��ϵ� ��ü�� ���̸� Ȯ���Ѵ�.
        float fDist = SHADOWMAP_P.Sample(g_sam_0, LocalDir).x;
            
        // ���� �������� ��ü�� ��ϵ� ����(�þ� ����) �̳������� �׽�Ʈ�� �����Ѵ�.
        if (length(LocalPos) < 0.5f)
        {
            // ������������ ��ϵ� ���̰���, ������ ���̸� ���Ѵ�.
            // ��ϵ� ���̺��� ���� ������Ų ���̰� �� ��ٸ�, ������������ ������ �����̴� ==> �׸��ڰ� ���ܾ� �Ѵ�.
            if (fDist + 0.0001f < vProjPos.z)
                discard;
        }
    }
    else if (g_LightBuffer[LIGHT_INDEX].Type == SPOT)
    {
        float4 vWorldPos = mul(ViewPos, g_matInvView);
        float4 vProjPos = mul(vWorldPos, g_matShadowVP_0);
        float3 LocalPos = mul(mul(ViewPos, g_matInvView), g_matInvWorld).xyz;
        float Angle = acos(dot(normalize(LocalPos), float3(0.f, 0.f, 1.f))) * (180.f / PI);
        vProjPos = vProjPos / vProjPos.w;
            
        // �������� ������Ų NDC ��ǥ�� UV �� ��ȯ�ؼ� ���������� ��ϵ� ��ü�� ���̸� Ȯ���Ѵ�.
        float2 vShadowMapUV = float2((vProjPos.x + 1.f) * 0.5f, 1.f - (vProjPos.y + 1.f) * 0.5f);
        float fDist = SHADOWMAP_S.Sample(g_sam_0, vShadowMapUV).x;
            
        // ���� �������� ��ü�� ��ϵ� ����(�þ� ����) �̳������� �׽�Ʈ�� �����Ѵ�.
        if (0.f < vShadowMapUV.x && vShadowMapUV.x < 1.f
            && 0.f < vShadowMapUV.y && vShadowMapUV.y < 1.f
            && length(LocalPos) < 0.5f && Angle < g_LightBuffer[LIGHT_INDEX].Angle / 2.f)
        {
            // ������������ ��ϵ� ���̰���, ������ ���̸� ���Ѵ�.
            // ��ϵ� ���̺��� ���� ������Ų ���̰� �� ��ٸ�, ������������ ������ �����̴� ==> �׸��ڰ� ���ܾ� �Ѵ�.
            if (fDist + 0.0001f < vProjPos.z)
                discard;
        }
    }
    
    return output;
}



#endif