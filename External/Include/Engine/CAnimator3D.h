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
    vector<Matrix>				m_vecFinalBoneMat;      // �ؽ��Ŀ� ������ ���� �������
    int							m_FrameCount;          // 30
    double						m_CurTime;
    int							m_CurClip;             // Ŭ�� �ε���	

    int							m_FrameIdx;            // Ŭ���� ���� ������
    int							m_NextFrameIdx;        // Ŭ���� ���� ������
    float						m_Ratio;	            // ������ ���� ����

    CStructuredBuffer*          m_BoneFinalMatBuffer;   // Ư�� �������� ���� ���
    bool						m_bFinalMatUpdate;      // ������� ���� ���࿩��


public:
    void SetBones(const vector<tMTBone>& _vecBones) { m_vecBones = &_vecBones; m_vecFinalBoneMat.resize(m_vecBones->size()); }
    void SetAnimClip(const vector<tMTAnimClip>& _vecAnimClip);
    void SetClipTime(int _iClipIdx, float _fTime) { m_vecClipUpdateTime[_iClipIdx] = _fTime; }

    CStructuredBuffer* GetFinalBoneMat() { return m_BoneFinalMatBuffer; }
    UINT GetBoneCount() { return (UINT)m_vecBones->size(); }
    void ClearData(AssetPtr<CMaterial> _Mtrl = nullptr);

    void Binding(AssetPtr<CMaterial> _Mtrl);

private:
    void check_mesh(AssetPtr<CMesh> _pMesh);

public:
    virtual void FinalTick() override;
    virtual int Save(fstream& _Stream) override;
    virtual int Load(fstream& _Stream) override;

};

