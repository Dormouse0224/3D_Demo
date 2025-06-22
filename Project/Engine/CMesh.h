#pragma once
#include "CAsset.h"

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

    ComPtr<ID3D11Buffer>		m_IB;
    D3D11_BUFFER_DESC           m_IBDesc;
    UINT                        m_IdxCount;

    Vtx*                        m_VtxSysMem;
    UINT*                       m_IdxSysMem;

public:
    int Create(Vtx* _VtxSysMem, UINT _VtxCount, UINT* _IdxSysMem, UINT _IdxCount);
    void Render();
    void Render_Particle(UINT _Count);

private:
    void Binding();

public:
    UINT GetVtxCount() { return m_VtxCount; }
    UINT GetIdxCount() { return m_IdxCount; }

    virtual int Save(const wstring& _FileName, bool _Update = false) override;
    virtual int Load(const wstring& _FilePath) override;

};

