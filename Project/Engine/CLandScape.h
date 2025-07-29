#pragma once
#include "CRenderComponent.h"

#include "CRaycastCS.h"
#include "CHeightMapCS.h"
#include "CWeightMapCS.h"

struct tRaycastOut
{
    Vec2    Location;
    UINT    Distance;
    int     Success;
};

enum class LANDSCAPE_MODE
{
    NONE,
    HEIGHTMAP,
    SPLATING,

    END
};

struct tWeight8
{
    float arrWeight[8];
};



class CLandScape :
    public CRenderComponent
{
public:
    CLandScape();
    CLandScape(const CLandScape& _Other);
    virtual ~CLandScape();
    CLONE(CLandScape);

private:
    UINT                        m_FaceX;    // ������ �� ����
    UINT                        m_FaceZ;    // ������ �� ����
    
    AssetPtr<CTexture>          m_ColorTex;
    AssetPtr<CTexture>          m_NormalTex;

    AssetPtr<CTexture>          m_HeightMap;
    AssetPtr<CHeightMapCS>      m_HeightMapCS;  // ���̸� ������ ��ǻƮ ���̴�

    // Raycasting
    AssetPtr<CRaycastCS>        m_RaycastCS;
    CStructuredBuffer*          m_RaycastOut;   // ����ĳ���� ��� ��� ����
    tRaycastOut                 m_Out;          // ����ĳ���� ��� ����ü

    vector<AssetPtr<CTexture>>  m_vecBrush;
    Vec2                        m_BrushScale;   // �귯�� ũ��
    UINT                        m_BrushIdx;     // �귯�� �ε���

    // WeightMap
    CStructuredBuffer*          m_WeightMap;
    UINT                        m_WeightWidth;
    UINT                        m_WeightHeight;
    AssetPtr<CWeightMapCS>      m_WeightMapCS;
    int                         m_WeightIdx;

    // Tessellation 
    float                       m_MinLevel;
    float                       m_MaxLevel;
    float                       m_MaxLevelRange;
    float                       m_MinLevelRange;

    // LandScape ���
    LANDSCAPE_MODE              m_Mode;

public:
    virtual void FinalTick() override;
    virtual void Render() override;

    virtual int Save(fstream& _Stream) override;
    virtual int Load(fstream& _Stream) override;

public:
    UINT GetFaceX() { return m_FaceX; }
    UINT GetFaceZ() { return m_FaceZ; }
    AssetPtr<CTexture> GetColorTexture() { return m_ColorTex; }
    AssetPtr<CTexture> GetNormalTexture() { return m_NormalTex; }
    LANDSCAPE_MODE GetMode() { return m_Mode; }
    vector<AssetPtr<CTexture>> GetBrushes() { return m_vecBrush; }
    UINT GetBrushIdx() { return m_BrushIdx; }
    int GetWeightIdx() { return m_WeightIdx; }

    void SetFace(UINT _X, UINT _Z);
    void SetColorTexture(AssetPtr<CTexture> _Tex);
    void SetNormalTexture(AssetPtr<CTexture> _Tex);
    void SetHeightMap(AssetPtr<CTexture> _HeightMap);
    void SetMode(LANDSCAPE_MODE _Mode);
    void SetBrushIdx(UINT _Idx);
    void SetWeightIdx(int _Idx);


    void CreateHeightMap(UINT _Width, UINT _Height);
    void CreateHeightMap(AssetPtr<CTexture> _Tex);
    void AddBrushTexture(AssetPtr<CTexture> _BrushTex) { m_vecBrush.push_back(_BrushTex); }

private:
    void CreateLandScapeMesh();
    void CreateComputeShader();
    void CreateWeightMap();
    int Raycasting();

};

