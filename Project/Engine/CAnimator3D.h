#pragma once
#include "CComponent.h"

#include "CMesh.h"

class CStructuredBuffer;
class CMaterial;

class CAnimator3D :
    public CComponent
{
public:
    CAnimator3D();
    CAnimator3D(const CAnimator3D& _Other);
    ~CAnimator3D();
    CLONE(CAnimator3D);

private:
    const vector<tMTBone>*      m_vecBones;
    const vector<tMTAnimClip>*  m_vecClip;

    vector<float>				m_vecClipUpdateTime;
    vector<Matrix>				m_vecFinalBoneMat;      // 텍스쳐에 전달할 최종 행렬정보
    int							m_FrameCount;          // 30
    double						m_CurTime;
    int							m_CurClip;             // 클립 인덱스	

    int							m_FrameIdx;            // 클립의 현재 프레임
    int							m_NextFrameIdx;        // 클립의 다음 프레임
    float						m_Ratio;	            // 프레임 사이 비율

    CStructuredBuffer*          m_BoneFinalMatBuffer;   // 특정 프레임의 최종 행렬
    bool						m_bFinalMatUpdate;      // 최종행렬 연산 수행여부
    
    Vec3                        m_LeftLegIK;
    Vec3                        m_RightLegIK;


public:
    void SetBones(const vector<tMTBone>& _vecBones) { m_vecBones = &_vecBones; m_vecFinalBoneMat.resize(m_vecBones->size()); }
    void SetAnimClip(const vector<tMTAnimClip>& _vecAnimClip);
    void SetClipTime(int _iClipIdx, float _fTime) { m_vecClipUpdateTime[_iClipIdx] = _fTime; }
    void SetCurClipIdx(int _Idx);
    void SetLeftLegIK(Vec3 _v) { m_LeftLegIK = _v; }
    void SetRightLegIK(Vec3 _v) { m_RightLegIK = _v; }

    CStructuredBuffer* GetFinalBoneMat() { return m_BoneFinalMatBuffer; }
    UINT GetBoneCount() { return (UINT)m_vecBones->size(); }
    int GetCurClipIdx() { return m_CurClip; }
    const vector<tMTAnimClip>* GetClipVec() { return m_vecClip; }
    Vec3 GetLeftLegIK() { return m_LeftLegIK; }
    Vec3 GetRightLegIK() { return m_RightLegIK; }

    void ClearData(AssetPtr<CMaterial> _Mtrl = nullptr);
    void Binding(AssetPtr<CMaterial> _Mtrl);

private:
    void check_mesh(AssetPtr<CMesh> _pMesh);
    void LegIKSolution(const tMTBone* _pBoneFoot, Vec3 _Offset);
    vector<Matrix> RotateBone(const tMTBone* _pBoneFoot, Matrix _RotMat, const vector<Matrix>& _vecMat);


public:
    virtual void FinalTick() override;
    virtual int Save(fstream& _Stream) override;
    virtual int Load(fstream& _Stream) override;

};

