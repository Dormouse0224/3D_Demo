#pragma once
#include "CEntity.h"


enum SB_TYPE
{
    SRV_ONLY,
    SRV_UAV,
};


class CStructuredBuffer :
    public CEntity
{
public:
    CStructuredBuffer();
    ~CStructuredBuffer();
    CLONE(CStructuredBuffer);

private:
    ComPtr<ID3D11Buffer>                m_SB;       // 메인 버퍼
    ComPtr<ID3D11Buffer>                m_SB_Write; // 쓰기 버퍼
    ComPtr<ID3D11Buffer>                m_SB_Read;  // 읽기 버퍼

    ComPtr<ID3D11ShaderResourceView>    m_SRV;  // t 레지스터 바인딩 리소스
    ComPtr<ID3D11UnorderedAccessView>   m_UAV;  // u 레지스터 바인딩 리소스 (ComputeShader 전용)
    D3D11_BUFFER_DESC                   m_Desc; // 버퍼 생성 옵션

    SB_TYPE                             m_Type;
    bool                                m_SysMemMove;

    UINT                                m_ElementSize;  // 데이터 1개 크기
    UINT                                m_ElementCount; // 데이터 개수

    int                                 m_RecentSRV_CS;
    int                                 m_RecentUAV_CS;

public:
    UINT GetElementSize() { return m_ElementSize; }
    UINT GetElementCount() { return m_ElementCount; }
    UINT GetBufferSize() { return m_ElementSize * m_ElementCount; }

public:
    int Create(UINT _ElementSize, UINT _ElementCount, SB_TYPE _Type, bool _SystemMemMove, void* _InitialData = nullptr);
    void SetData(void* _Data, UINT _ElementSize = 0, UINT _ElementCount = 0);
    void GetData(void* _Dst, UINT _ElementSize = 0, UINT _ElementCount = 0);

    void Binding(int _RegisterNum);
    void Unbind(int _RegisterNum);

    void Binding_CS_SRV(int _RegisterNum);
    void Unbind_CS_SRV();

    void Binding_CS_UAV(int _RegisterNum);
    void Unbind_CS_UAV();


};

