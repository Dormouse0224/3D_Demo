#include "pch.h"
#include "CConstBuffer.h"

#include "CDevice.h"

CConstBuffer::CConstBuffer()
	: m_Desc{}
	, m_Type(CB_TYPE::END)
{
}

CConstBuffer::~CConstBuffer()
{
}

int CConstBuffer::Create(CB_TYPE _Type, UINT _BufferSize)
{
	m_Type = _Type;

	// 상수버퍼 생성
	m_Desc.ByteWidth = _BufferSize;

	// 생성 이후엔 데이터 이동 없음
	m_Desc.Usage = D3D11_USAGE_DYNAMIC;
	m_Desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	m_Desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

	HRESULT err;
	if (FAILED(err = CDevice::GetInst()->GetDevice().Get()->CreateBuffer(&m_Desc, nullptr, m_Buffer.GetAddressOf())))
	{
		return E_FAIL;
	}

	DEBUG_NAME(m_Buffer, "CConstBuffer_Buffer");

	return S_OK;
}

void CConstBuffer::SetData(void* _Data, UINT _DataSize)
{
	D3D11_MAPPED_SUBRESOURCE tMapSub = {};

	// GPU 메모리 잠금 및 CPU로 매핑
	CONTEXT->Map(m_Buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &tMapSub);
	// 매핑된 주소에 데이터 복사
	memcpy(tMapSub.pData, _Data, _DataSize);
	// 언맵 및 잠금해제
	CONTEXT->Unmap(m_Buffer.Get(), 0);
}

void CConstBuffer::Binding()
{
	CONTEXT->VSSetConstantBuffers((UINT)m_Type, 1, m_Buffer.GetAddressOf());
	CONTEXT->HSSetConstantBuffers((UINT)m_Type, 1, m_Buffer.GetAddressOf());
	CONTEXT->DSSetConstantBuffers((UINT)m_Type, 1, m_Buffer.GetAddressOf());
	CONTEXT->GSSetConstantBuffers((UINT)m_Type, 1, m_Buffer.GetAddressOf());
	CONTEXT->PSSetConstantBuffers((UINT)m_Type, 1, m_Buffer.GetAddressOf());
}


void CConstBuffer::Binding_CS()
{
	CONTEXT->CSSetConstantBuffers((UINT)m_Type, 1, m_Buffer.GetAddressOf());
}
