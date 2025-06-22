#pragma once
#include "CEntity.h"

class CConstBuffer :
    public CEntity
{
public:
    CConstBuffer();
    ~CConstBuffer();
    CLONE_DISABLE(CConstBuffer);

private:
    ComPtr<ID3D11Buffer>    m_Buffer;
    D3D11_BUFFER_DESC       m_Desc;

    CB_TYPE             m_Type;

public:
    int Create(CB_TYPE _Type, UINT _BufferSize);
    /// <summary>
    /// 상수 버퍼에 데이터를 작성합니다.
    /// </summary>
    /// <param name="_Data">작성할 데이터</param>
    /// <param name="_DataSize">작성할 데이터의 크기</param>
    void SetData(void* _Data, UINT _DataSize);
    void Binding();
    void Binding_CS();
};

