#include "pch.h"
#include "CComputeShader.h"

#include "CDevice.h"
#include "CConstBuffer.h"
#include "CPathMgr.h"

CComputeShader::CComputeShader()
	: CShader(ASSET_TYPE::COMPUTE_SHADER)
	, m_ThreadPerGroupX(1)
	, m_ThreadPerGroupY(1)
	, m_ThreadPerGroupZ(1)
	, m_GroupX(1)
	, m_GroupY(1)
	, m_GroupZ(1)
	, m_CSTex{}
{
}

CComputeShader::CComputeShader(const wstring& _RelativePath, const string& _FuncName
	, UINT _GroupPerX, UINT _GroupPerY, UINT _GroupPerZ)
	: CShader(ASSET_TYPE::COMPUTE_SHADER)
	, m_ThreadPerGroupX(_GroupPerX)
	, m_ThreadPerGroupY(_GroupPerY)
	, m_ThreadPerGroupZ(_GroupPerZ)
	, m_GroupX(1)
	, m_GroupY(1)
	, m_GroupZ(1)
	, m_CSTex{}
{
	CreateComputeShader(_RelativePath, _FuncName);
}

CComputeShader::~CComputeShader()
{
}

int CComputeShader::CreateComputeShader(const wstring& _RelativePath, const string& _FuncName)
{
	wstring strFilePath = CPathMgr::GetContentDir() + _RelativePath;

	UINT Flag = 0;

#ifdef _DEBUG
	Flag = D3DCOMPILE_DEBUG;
#endif

	if (FAILED(D3DCompileFromFile(strFilePath.c_str()
		, nullptr
		, D3D_COMPILE_STANDARD_FILE_INCLUDE
		, _FuncName.c_str(), "cs_5_0", Flag, 0
		, m_CSBlob.GetAddressOf()
		, m_ErrBlob.GetAddressOf())))
	{
		// 컴파일 실패하면 메세지 박스에 컴파일 실패내용 띄우기
		if (nullptr != m_ErrBlob)
		{
			char* pErrMsg = (char*)m_ErrBlob->GetBufferPointer();
			MessageBoxA(nullptr, pErrMsg, "쉐이더 컴파일 실패", MB_OK);
		}
		else
		{
			MessageBoxA(nullptr, "쉐이더 파일 경로가 옮바르지 않습니다.", "쉐이더 컴파일 실패", MB_OK);
		}
		return E_FAIL;
	}

	if (FAILED(DEVICE->CreateComputeShader(m_CSBlob->GetBufferPointer()
		, m_CSBlob->GetBufferSize()
		, nullptr
		, m_CS.GetAddressOf())))
	{
		return E_FAIL;
	}

	return S_OK;
}

int CComputeShader::Execute()
{
	// 리소스 및 쉐이더 바인딩
	if (FAILED(Binding()))
	{
		assert(nullptr);
		return E_FAIL;
	}

	CalcGroupNum();

	// 상수값을 상수버퍼에 전달 및 CS 바인딩
	static CConstBuffer* pBuffer = CDevice::GetInst()->GetConstBuffer(CB_TYPE::MATERIAL);
	pBuffer->SetData(&m_Const, sizeof(tMtrlConst));
	pBuffer->Binding_CS();

	// 텍스처 바인딩
	for (int i = 0; i < TEX_PARAM::TEX_END; ++i)
	{
		if (m_CSTex[i].first != nullptr)
			m_CSTex[i].first->Binding_CS_SRV(m_CSTex[i].second);
	}

	// 컴퓨트 쉐이더 바인딩 및 실행
	CONTEXT->CSSetShader(m_CS.Get(), nullptr, 0);
	CONTEXT->Dispatch(m_GroupX, m_GroupY, m_GroupZ);

	// 리소스 및 쉐이더 바인딩 클리어
	Clear();
	for (int i = 0; i < TEX_PARAM::TEX_END; ++i)
	{
		if (m_CSTex[i].first != nullptr)
			m_CSTex[i].first->Unbind_CS_SRV(m_CSTex[i].second);
	}
	CONTEXT->CSSetShader(nullptr, nullptr, 0);
}
