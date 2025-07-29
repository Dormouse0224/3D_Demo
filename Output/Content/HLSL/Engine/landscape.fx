#ifndef _LANDSCAPE
#define _LANDSCAPE

#include "value.fx"
#include "func.fx"

#define FACE_X              g_int_0
#define FACE_Z              g_int_1
#define MODE                g_int_2
#define TEXTURE_ARRSIZE     g_int_3
#define MAX_HEIGHT          g_float_1

#define IsShowBrush         g_btex_1 && g_float_0
#define BRUSH_TEX           g_tex_1
#define BrushScale          g_vec2_0
#define BrushPos            g_vec2_1

#define IsHeightMap         g_btex_0
#define HEIGHT_MAP          g_tex_0

StructuredBuffer<tWeight8>  WEIGHT_MAP : register(t20);
#define WEIGHT_RESOLUTION   g_vec2_2

#define COLOR_TEX           g_texarr_0
#define NORMAL_TEX          g_texarr_1
#define HasColorTex         g_btexArr_0
#define HasNormalTex        g_btexArr_1

#define MIN_LEVEL           g_vec4_0.x
#define MAX_LEVEL           g_vec4_0.y
#define MIN_RANGE           g_vec4_0.z
#define MAX_RANGE           g_vec4_0.w
#define CAM_POS             g_vec4_1.xyz

struct VS
{
    float3 vPos : POSITION;
    float2 vUV : TEXCOORD;  
};

struct VS_OUT
{
    float3 vPos : POSITION;
    float3 vWorldPos : POSITION1;
    float2 vUV : TEXCOORD;
};

VS_OUT VS_LandScape(VS _in)
{
    VS_OUT output = (VS_OUT) 0.f;
    
    output.vPos = _in.vPos;
    output.vUV = _in.vUV;
    output.vWorldPos = mul(float4(_in.vPos, 1.f), g_matWorld).xyz;
    
    return output;
}

struct PatchLevel
{
    float Edge[3] : SV_TessFactor;
    float Inside : SV_InsideTessFactor;
};

// Patch : 최소한의 도형, 삼각형
// 패치상수함수, 패치 당 1번만 실행
PatchLevel PatchConstFunc(InputPatch<VS_OUT, 3> Patch, uint PatchID : SV_PrimitiveID)
{
    PatchLevel output = (PatchLevel) 0.f;
    
    float TessLevel = 1.f;    
    
    if (0 != g_float_0)
        TessLevel = g_float_0;
       
    
    // 위, 아래
    output.Edge[0] = GetTessFactor(MIN_LEVEL, MAX_LEVEL
                                 , MIN_RANGE, MAX_RANGE, CAM_POS
                                 , (Patch[1].vWorldPos + Patch[2].vWorldPos) / 2.f);
    
    // 좌, 우
    output.Edge[1] = GetTessFactor(MIN_LEVEL, MAX_LEVEL
                                 , MIN_RANGE, MAX_RANGE, CAM_POS
                                 , (Patch[0].vWorldPos + Patch[2].vWorldPos) / 2.f);
    
    // 빗변
    output.Edge[2] = GetTessFactor(MIN_LEVEL, MAX_LEVEL
                                 , MIN_RANGE, MAX_RANGE, CAM_POS
                                 , (Patch[0].vWorldPos + Patch[1].vWorldPos) / 2.f);
    
    // 삼각형 중심
    output.Inside = GetTessFactor(MIN_LEVEL, MAX_LEVEL
                                , MIN_RANGE, MAX_RANGE, CAM_POS
                                , (Patch[0].vWorldPos + Patch[1].vWorldPos + Patch[2].vWorldPos) / 3.f);
    
    return output;
}

// 패치를 구성하고있는 정점 하나당 한번씩 수행
[domain("tri")]
//[partitioning("integer")]
[partitioning("fractional_odd")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(3)]
[patchconstantfunc("PatchConstFunc")]
[maxtessfactor(32.f)]
VS_OUT HS_LandScape(InputPatch<VS_OUT, 3> Patch,
                 uint i : SV_OutputControlPointID,
                 uint PatchID : SV_PrimitiveID)
{
    return Patch[i];
}


// Domain Shader
struct DS_OUT
{
    float4 vPosition : SV_Position;
    float2 vUV : TEXCOORD;
    
    float3 vViewPos : POSITION;
    float3 vViewNormal : NORMAL;
    float3 vViewTangent : TANGENT;
    float3 vViewBinormal : BINORMAL;   
};

[domain("tri")]
DS_OUT DS_LandScape(PatchLevel _PatchLevel, OutputPatch<VS_OUT, 3> _OutPatch, float3 _Weight : SV_DomainLocation)
{
    DS_OUT output = (DS_OUT) 0.f;
    
    VS Input = (VS) 0.f;    
    
    for (int i = 0; i < 3; ++i)
    {
        Input.vPos += _OutPatch[i].vPos * _Weight[i];
        Input.vUV += _OutPatch[i].vUV * _Weight[i];          
    }
    
    float3 vNormal = float3(0.f, 1.f, 0.f);
    float3 vTangent = float3(1.f, 0.f, 0.f);
    float3 vBinormal = float3(0.f, 0.f, -1.f);
        
    float2 FullUV = Input.vUV / float2((float) FACE_X, (float) FACE_Z);
    
    // 높이맵에서 높이값 추출
    // 지형 전체를 0 ~ 1로 봤을때 기준, 해당 정점의 UV 값
    if (IsHeightMap)
    {        
        Input.vPos.y = HEIGHT_MAP.SampleLevel(g_sam_2, FullUV, 0).r * MAX_HEIGHT;
        
        float LocalStep = 1.f / _PatchLevel.Inside;        
        float2 UVStep = float2(LocalStep, LocalStep) / float2((float) FACE_X, (float) FACE_Z);        
                
        float3 Up = float3(Input.vPos.x, HEIGHT_MAP.SampleLevel(g_sam_2, FullUV + float2(0.f, -UVStep.y), 0).r, Input.vPos.z + LocalStep);
        float3 Down = float3(Input.vPos.x, HEIGHT_MAP.SampleLevel(g_sam_2, FullUV + float2(0.f, UVStep.y), 0).r, Input.vPos.z - LocalStep);
        float3 Left = float3(Input.vPos.x - LocalStep, HEIGHT_MAP.SampleLevel(g_sam_2, FullUV + float2(-UVStep.x, 0.f), 0).r, Input.vPos.z);
        float3 Right = float3(Input.vPos.x + LocalStep, HEIGHT_MAP.SampleLevel(g_sam_2, FullUV + float2(UVStep.x, 0.f), 0).r, Input.vPos.z);
        
        vTangent = normalize(Right - Left);
        vBinormal = normalize(Down - Up);
        vNormal = normalize(cross(vTangent, vBinormal));
    }
    
    output.vPosition = mul(float4(Input.vPos, 1.f), g_matWVP);
    output.vUV = Input.vUV;
        
    output.vViewPos = mul(float4(Input.vPos, 1.f), g_matWV).xyz;
    output.vViewTangent = normalize(mul(float4(vTangent, 0.f), g_matWV)).xyz;
    output.vViewNormal = normalize(mul(float4(vNormal, 0.f), g_matWV)).xyz;
    output.vViewBinormal = normalize(mul(float4(vBinormal, 0.f), g_matWV)).xyz;
    
    return output;    
}


struct PS_OUT
{
    float4 vColor : SV_Target;
    float4 vNormal : SV_Target1;
    float4 vPosition : SV_Target2;
    float4 vEmissve : SV_Target3;
    float4 vData : SV_Target4;
};

PS_OUT PS_LandScape(DS_OUT _in)
{
    PS_OUT output = (PS_OUT) 0.f;
    
    float4 vBrush = (float4) 0.f;
    
    if (IsShowBrush && MODE)
    {
        // Brush LeftTop 좌표
        float2 BrushLT = BrushPos - (BrushScale * 0.5f);
        
        // 지형 기준, 픽셀의 위치 구하기
        float2 vBrusUV = _in.vUV / float2(FACE_X, FACE_Z);
        vBrusUV = (vBrusUV - BrushLT) / BrushScale;
        
        if (0.f <= vBrusUV.x && vBrusUV.x <= 1.f
            && 0.f <= vBrusUV.y && vBrusUV.y <= 1.f)
        {
            float BrushAlpha = BRUSH_TEX.Sample(g_sam_0, vBrusUV).a;
            float3 BrushColor = float3(0.8f, 0.8f, 0.f);
            
            vBrush.rgb = (vBrush.rgb * (1 - BrushAlpha)) + (BrushColor * BrushAlpha);
        }
    }
       
    
    float4 vColor = float4(0.7f, 0.7f, 0.7f, 1.f);
    float3 vViewNormal = _in.vViewNormal;
        
    if (HasColorTex)
    {
        float2 vFullUV = _in.vUV / float2(FACE_X, FACE_Z);
        int2 vColRow = vFullUV * WEIGHT_RESOLUTION;
        int WeightMapIdx = WEIGHT_RESOLUTION.x * vColRow.y + vColRow.x;

        vColor = (float4) 0.f;
        
        int MaxIdx = -1;
        float WeightMax = 0.f;
                
        // 편미분, 주변 픽셀과의 입력값의 변화량
        float2 derivX = ddx(_in.vUV);
        float2 derivY = ddy(_in.vUV);
        
        for (int i = 0; i < TEXTURE_ARRSIZE; ++i)
        {
            float Weight = WEIGHT_MAP[WeightMapIdx].arrWeight[i];
            
            if (0.f != Weight)
            {
                //vColor += COLOR_TEX.SampleLevel(g_sam_0, float3(_in.vUV, i), 4) * Weight;
                vColor += COLOR_TEX.SampleGrad(g_sam_0, float3(_in.vUV, i), derivX * 0.3f, derivY * 0.3f) * Weight;
            }
                        
            // 제일 높았던 가중치를 기록
            if (WeightMax < Weight)
            {
                WeightMax = Weight;
                MaxIdx = i;
            }
        }
        
        if (MaxIdx != -1)
        {
            //float3 vNormal = NORMAL_TEX.SampleLevel(g_sam_0, float3(_in.vUV, MaxIdx), 4);
            float3 vNormal = NORMAL_TEX.SampleGrad(g_sam_0, float3(_in.vUV, MaxIdx), derivX * 0.3f, derivY * 0.3f).xyz;
            vNormal = vNormal * 2.f - 1.f;
        
            float3x3 Rot =
            {
                _in.vViewTangent,
                _in.vViewBinormal,
                _in.vViewNormal
            };
        
            vViewNormal = normalize(mul(vNormal, Rot));
        }
    }
    
    output.vColor = float4(vColor.xyz + vBrush.rgb * 0.5f, 1.f);
    output.vEmissve = float4(vBrush.rgb, 1.f);
    output.vNormal = float4(vViewNormal, 1.f);
    output.vPosition = float4(_in.vViewPos, 1.f);
    
    return output;
}



#endif