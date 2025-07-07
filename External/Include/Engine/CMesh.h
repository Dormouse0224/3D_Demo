#pragma once
#include "CAsset.h"
#include "CFBXLoader.h"
#include "CStructuredBuffer.h"

struct tIndexInfo
{
    ComPtr<ID3D11Buffer>    IB;
    D3D11_BUFFER_DESC       tIBDesc;
    UINT				    IdxCount;
    void*                   IdxSysMem;
};

class CMesh :
    public CAsset
{
public:
    CMesh();
    ~CMesh();
    CLONE_DISABLE(CMesh);

private:
    ComPtr<ID3D11Buffer>		m_VB;
    D3D11_BUFFER_DESC           m_VBDesc;
    UINT                        m_VtxCount;
    Vtx*                        m_VtxSysMem;

    // �ϳ��� ���ؽ����ۿ� �������� �ε������۰� ����
    vector<tIndexInfo>		    m_vecIdxInfo;

    // Animation3D ����
    vector<tMTAnimClip>		    m_vecAnimClip;
    vector<tMTBone>			    m_vecBones;

    CStructuredBuffer*          m_BoneFrameData;    // ��ü �� ������ ����(ũ��, �̵�, ȸ��) (������ ������ŭ)
    CStructuredBuffer*          m_BoneInverse;	    // �� ���� offset ���(�� ���� ��ġ�� �ǵ����� ���) (1�� ¥��)

public:
    int Create(Vtx* _VtxSysMem, UINT _VtxCount, UINT* _IdxSysMem, UINT _IdxCount);
    static CMesh* CreateFromContainer(CFBXLoader& _loader);

    void Render(UINT _iSubset = 0);
    void Render_Particle(UINT _Count);

private:
    void Binding(UINT _iSubset);

public:
    UINT GetVtxCount() { return m_VtxCount; }
    UINT GetIdxCount() { return (UINT)m_vecIdxInfo.size(); }
    void* GetVtxSysMem() { return m_VtxSysMem; }

    const vector<tMTBone>& GetBones() { return m_vecBones; }
    UINT GetBoneCount() { return (UINT)m_vecBones.size(); }
    const vector<tMTAnimClip>& GetAnimClip() { return m_vecAnimClip; }
    CStructuredBuffer* GetBoneFrameDataBuffer() { return m_BoneFrameData; } // ��ü �� ������ ����
    CStructuredBuffer* GetBoneInverseBuffer() { return  m_BoneInverse; }	   // �� Bone �� Inverse ���

    bool IsAnimMesh() { return !m_vecAnimClip.empty(); }

    virtual int Save(const wstring& _FileName, bool _Update = false) override;
    virtual int Load(const wstring& _FilePath) override;

};

