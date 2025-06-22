#include "pch.h"
#include "CStructuredBuffer.h"

#include "CDevice.h"

CStructuredBuffer::CStructuredBuffer()
	: m_Desc{}
	, m_Type(SRV_ONLY)
	, m_ElementSize(0)
	, m_ElementCount(0)
{
}

CStructuredBuffer::~CStructuredBuffer()
{
	m_SB.Reset();
	m_SB_Write.Reset();
	m_SB_Read.Reset();
	m_SRV.Reset();
	m_UAV.Reset();
}

int CStructuredBuffer::Create(UINT _ElementSize, UINT _ElementCount
	, SB_TYPE _Type, bool _SystemMemMove, void* _InitialData)
{
	m_SB = nullptr;
	m_SB_Write = nullptr;
	m_SB_Read = nullptr;

	m_SRV = nullptr;
	m_UAV = nullptr;

	m_Type = _Type;
	m_SysMemMove = _SystemMemMove;

	m_ElementSize = _ElementSize;
	m_ElementCount = _ElementCount;

	// 버퍼 크기의 단위가 16 배수여야 한다.
	int padding_byte = (m_ElementSize * m_ElementCount) % 16 ? 16 - (m_ElementSize * m_ElementCount) % 16 : (m_ElementSize * m_ElementCount) % 16;
	assert(!padding_byte);

	// 버퍼 전체 용량
	m_Desc.ByteWidth = m_ElementSize * m_ElementCount;

	// 메인 버퍼는 SystemMem 랑 통신 불가능
	m_Desc.Usage = D3D11_USAGE_DEFAULT;
	m_Desc.CPUAccessFlags = 0;

	// 데이터 요소 크기(간격)
	m_Desc.StructureByteStride = m_ElementSize;

	// 구조화버퍼 전용 플래그
	m_Desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;

	// 용도
	if (SB_TYPE::SRV_ONLY == m_Type)
		m_Desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	else
		m_Desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;

	// 메인 버퍼 생성하기
	HRESULT hr = S_OK;
	if (nullptr == _InitialData)
	{
		hr = DEVICE->CreateBuffer(&m_Desc, nullptr, m_SB.GetAddressOf());
	}
	else
	{
		D3D11_SUBRESOURCE_DATA tSub = {};
		tSub.pSysMem = _InitialData;
		hr = DEVICE->CreateBuffer(&m_Desc, &tSub, m_SB.GetAddressOf());
	}

	if (FAILED((hr)))
		return E_FAIL;

	// t 레지스터 바인딩용, ShaderResourceView 생성
	D3D11_SHADER_RESOURCE_VIEW_DESC tSRVDesc = {};

	tSRVDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
	tSRVDesc.Buffer.NumElements = _ElementCount;

	if (FAILED(DEVICE->CreateShaderResourceView(m_SB.Get(), &tSRVDesc, m_SRV.GetAddressOf())))
	{
		return E_FAIL;
	}

	// UAV 추가 생성
	if (SB_TYPE::SRV_UAV == m_Type)
	{
		// u 레지스터 바인딩 용, UnorderedAccessView 생성
		D3D11_UNORDERED_ACCESS_VIEW_DESC tUAVDesc = {};

		tUAVDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
		tUAVDesc.Buffer.NumElements = _ElementCount;

		if (FAILED(DEVICE->CreateUnorderedAccessView(m_SB.Get(), &tUAVDesc, m_UAV.GetAddressOf())))
		{
			return E_FAIL;
		}
	}

	// SystemMem 데이터 읽기 쓰기용 버퍼 2개 생성
	if (m_SysMemMove)
	{
		D3D11_BUFFER_DESC tWriteBufferDesc = {};
		tWriteBufferDesc.ByteWidth = m_ElementSize * m_ElementCount;
		tWriteBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		tWriteBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		tWriteBufferDesc.StructureByteStride = m_ElementSize;
		tWriteBufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
		tWriteBufferDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

		hr = DEVICE->CreateBuffer(&tWriteBufferDesc, nullptr, m_SB_Write.GetAddressOf());


		D3D11_BUFFER_DESC tReadBufferDesc = {};
		tReadBufferDesc.ByteWidth = m_ElementSize * m_ElementCount;
		tReadBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		tReadBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
		tReadBufferDesc.StructureByteStride = m_ElementSize;
		tReadBufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
		tReadBufferDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

		hr = DEVICE->CreateBuffer(&tReadBufferDesc, nullptr, m_SB_Read.GetAddressOf());
	}

	return S_OK;
}

void CStructuredBuffer::SetData(void* _Data, UINT _ElementSize, UINT _ElementCount)
{
	assert(m_SysMemMove);

	if (_ElementSize == 0 && _ElementCount == 0)
	{
		_ElementSize = m_ElementSize;
		_ElementCount = m_ElementCount;
	}

	// SystemMem -> SB_Write
	D3D11_MAPPED_SUBRESOURCE tSubRes = {};
	CONTEXT->Map(m_SB_Write.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &tSubRes);
	memcpy(tSubRes.pData, _Data, _ElementSize * _ElementCount);
	CONTEXT->Unmap(m_SB_Write.Get(), 0);

	// SB_Write -> SB_Main
	CONTEXT->CopyResource(m_SB.Get(), m_SB_Write.Get());
}

void CStructuredBuffer::GetData(void* _Dst, UINT _ElementSize, UINT _ElementCount)
{
	if (_ElementSize == 0)
		_ElementSize = m_ElementSize;

	if (_ElementCount == 0)
		_ElementCount = m_ElementCount;

	// SB_Main -> SB_Read
	CONTEXT->CopyResource(m_SB_Read.Get(), m_SB.Get());

	// SB_Read -> SystemMem
	D3D11_MAPPED_SUBRESOURCE tSubRes = {};
	CONTEXT->Map(m_SB_Read.Get(), 0, D3D11_MAP_READ, 0, &tSubRes);
	memcpy(_Dst, tSubRes.pData, _ElementSize * _ElementCount);
	CONTEXT->Unmap(m_SB_Read.Get(), 0);
}

void CStructuredBuffer::Binding(int _RegisterNum)
{
	CONTEXT->VSSetShaderResources(_RegisterNum, 1, m_SRV.GetAddressOf());
	CONTEXT->HSSetShaderResources(_RegisterNum, 1, m_SRV.GetAddressOf());
	CONTEXT->DSSetShaderResources(_RegisterNum, 1, m_SRV.GetAddressOf());
	CONTEXT->GSSetShaderResources(_RegisterNum, 1, m_SRV.GetAddressOf());
	CONTEXT->PSSetShaderResources(_RegisterNum, 1, m_SRV.GetAddressOf());
}

void CStructuredBuffer::Unbind(int _RegisterNum)
{
	ID3D11ShaderResourceView* pSRV = nullptr;
	CONTEXT->VSSetShaderResources(_RegisterNum, 1, &pSRV);
	CONTEXT->HSSetShaderResources(_RegisterNum, 1, &pSRV);
	CONTEXT->DSSetShaderResources(_RegisterNum, 1, &pSRV);
	CONTEXT->GSSetShaderResources(_RegisterNum, 1, &pSRV);
	CONTEXT->PSSetShaderResources(_RegisterNum, 1, &pSRV);
}



void CStructuredBuffer::Binding_CS_SRV(int _RegisterNum)
{
	m_RecentSRV_CS = _RegisterNum;
	CONTEXT->CSSetShaderResources(_RegisterNum, 1, m_SRV.GetAddressOf());
}

void CStructuredBuffer::Binding_CS_UAV(int _RegisterNum)
{
	m_RecentUAV_CS = _RegisterNum;
	UINT i = -1;
	CONTEXT->CSSetUnorderedAccessViews(_RegisterNum, 1, m_UAV.GetAddressOf(), &i);
}

void CStructuredBuffer::Unbind_CS_SRV()
{
	ID3D11ShaderResourceView* pSRV = nullptr;
	CONTEXT->CSSetShaderResources(m_RecentSRV_CS, 1, &pSRV);
	m_RecentSRV_CS = -1;
}

void CStructuredBuffer::Unbind_CS_UAV()
{
	ID3D11UnorderedAccessView* pUAV = nullptr;
	UINT i = -1;
	CONTEXT->CSSetUnorderedAccessViews(m_RecentUAV_CS, 1, &pUAV, &i);
	m_RecentUAV_CS = -1;
}
