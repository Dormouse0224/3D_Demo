#include "pch.h"
#include "CGraphicShader.h"

#include "CPathMgr.h"
#include "CAssetMgr.h"
#include "CDevice.h"

string SHADER_TYPE_STR[] =
{
	"VERTEX_SHADER",
	"GEOMETRY_SHADER",
	"PIXEL_SHADER",

	"SHADER_TYPE_END"
};

CGraphicShader::CGraphicShader()
	: CShader(ASSET_TYPE::GRAPHIC_SHADER)
	, m_Topology(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST)
	, m_Domain(SHADER_DOMAIN::DOMAIN_QPAQUE)
	, m_RSType(RS_TYPE::CULL_BACK)
	, m_DSType(DS_TYPE::LESS)
	, m_BSType(BS_TYPE::DEFAULT)
	, m_ShaderName{}
{
}

CGraphicShader::~CGraphicShader()
{
}

int CGraphicShader::CreateVertexShader(const wstring& _RelativePath, const string& _FuncName)
{
	wstring strFilePath = CPathMgr::GetContentDir() + _RelativePath;

	UINT Flag = 0;

#ifdef _DEBUG
	Flag = D3DCOMPILE_DEBUG;
#endif

	if (FAILED(D3DCompileFromFile(strFilePath.c_str()
		, nullptr
		, D3D_COMPILE_STANDARD_FILE_INCLUDE
		, _FuncName.c_str(), "vs_5_0", Flag, 0
		, m_VSBlob.GetAddressOf()
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
			MessageBoxA(nullptr, "쉐이더 파일 경로가 올바르지 않습니다.", "쉐이더 컴파일 실패", MB_OK);
		}
		return E_FAIL;
	}

	if (FAILED(DEVICE->CreateVertexShader(m_VSBlob->GetBufferPointer()
		, m_VSBlob->GetBufferSize()
		, nullptr
		, m_VS.GetAddressOf())))
	{
		return E_FAIL;
	}

	m_ShaderName[VERTEX_SHADER] = make_pair(_RelativePath, _FuncName);

	// Input Layout 도 같이 생성	
	D3D11_INPUT_ELEMENT_DESC LayoutDesc[6] = {};

	LayoutDesc[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;			// 해당 데이터의 바이트 구조 및 크기
	LayoutDesc[0].InputSlot = 0;								// 버퍼의 인덱스 - 여러 버퍼를 동시에 전달할 때 사용
	LayoutDesc[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	LayoutDesc[0].InstanceDataStepRate = 0;
	LayoutDesc[0].SemanticName = "POSITION";
	LayoutDesc[0].SemanticIndex = 0;							// 시멘틱 인덱스 - 같은 시멘틱 데이터가 한 정점에 여러개 있을 때 사용
	LayoutDesc[0].AlignedByteOffset = 0;						// 해당 데이터의 정점 내 시작 위치 (바이트)

	LayoutDesc[1].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	LayoutDesc[1].InputSlot = 0;
	LayoutDesc[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	LayoutDesc[1].InstanceDataStepRate = 0;
	LayoutDesc[1].SemanticName = "COLOR";
	LayoutDesc[1].SemanticIndex = 0;
	LayoutDesc[1].AlignedByteOffset = 12;

	LayoutDesc[2].Format = DXGI_FORMAT_R32G32_FLOAT;
	LayoutDesc[2].InputSlot = 0;
	LayoutDesc[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	LayoutDesc[2].InstanceDataStepRate = 0;
	LayoutDesc[2].SemanticName = "TEXCOORD";
	LayoutDesc[2].SemanticIndex = 0;
	LayoutDesc[2].AlignedByteOffset = 28;

	LayoutDesc[3].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	LayoutDesc[3].InputSlot = 0;
	LayoutDesc[3].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	LayoutDesc[3].InstanceDataStepRate = 0;
	LayoutDesc[3].SemanticName = "TANGENT";
	LayoutDesc[3].SemanticIndex = 0;
	LayoutDesc[3].AlignedByteOffset = 36;

	LayoutDesc[4].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	LayoutDesc[4].InputSlot = 0;
	LayoutDesc[4].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	LayoutDesc[4].InstanceDataStepRate = 0;
	LayoutDesc[4].SemanticName = "NORMAL";
	LayoutDesc[4].SemanticIndex = 0;
	LayoutDesc[4].AlignedByteOffset = 48;

	LayoutDesc[5].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	LayoutDesc[5].InputSlot = 0;
	LayoutDesc[5].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	LayoutDesc[5].InstanceDataStepRate = 0;
	LayoutDesc[5].SemanticName = "BINORMAL";
	LayoutDesc[5].SemanticIndex = 0;
	LayoutDesc[5].AlignedByteOffset = 60;


	if (FAILED(DEVICE->CreateInputLayout(LayoutDesc, 6
		, m_VSBlob->GetBufferPointer()
		, m_VSBlob->GetBufferSize(), m_Layout.GetAddressOf())))
	{
		return E_FAIL;
	}

	return S_OK;
}

int CGraphicShader::CreateGeometryShader(const wstring& _RelativePath, const string& _FuncName)
{
	wstring strFilePath = CPathMgr::GetContentDir() + _RelativePath;

	UINT Flag = 0;

#ifdef _DEBUG
	Flag = D3DCOMPILE_DEBUG;
#endif

	if (FAILED(D3DCompileFromFile(strFilePath.c_str()
		, nullptr
		, D3D_COMPILE_STANDARD_FILE_INCLUDE
		, _FuncName.c_str(), "gs_5_0", Flag, 0
		, m_GSBlob.GetAddressOf()
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
			MessageBoxA(nullptr, "쉐이더 파일 경로가 올바르지 않습니다.", "쉐이더 컴파일 실패", MB_OK);
		}
		return E_FAIL;
	}

	if (FAILED(DEVICE->CreateGeometryShader(m_GSBlob->GetBufferPointer()
		, m_GSBlob->GetBufferSize()
		, nullptr
		, m_GS.GetAddressOf())))
	{
		return E_FAIL;
	}

	m_ShaderName[GEOMETRY_SHADER] = make_pair(_RelativePath, _FuncName);

	return S_OK;
}

int CGraphicShader::CreateDomainShader(const wstring& _RelativePath, const string& _FuncName)
{
	wstring strFilePath = CPathMgr::GetContentDir() + _RelativePath;

	UINT Flag = 0;

#ifdef _DEBUG
	Flag = D3DCOMPILE_DEBUG;
#endif

	if (FAILED(D3DCompileFromFile(strFilePath.c_str()
		, nullptr
		, D3D_COMPILE_STANDARD_FILE_INCLUDE
		, _FuncName.c_str(), "gs_5_0", Flag, 0
		, m_DSBlob.GetAddressOf()
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
			MessageBoxA(nullptr, "쉐이더 파일 경로가 올바르지 않습니다.", "쉐이더 컴파일 실패", MB_OK);
		}
		return E_FAIL;
	}

	if (FAILED(DEVICE->CreateDomainShader(m_DSBlob->GetBufferPointer()
		, m_DSBlob->GetBufferSize()
		, nullptr
		, m_DS.GetAddressOf())))
	{
		return E_FAIL;
	}

	m_ShaderName[DOMAIN_SHADER] = make_pair(_RelativePath, _FuncName);

	return S_OK;
}

int CGraphicShader::CreateHullShader(const wstring& _RelativePath, const string& _FuncName)
{
	wstring strFilePath = CPathMgr::GetContentDir() + _RelativePath;

	UINT Flag = 0;

#ifdef _DEBUG
	Flag = D3DCOMPILE_DEBUG;
#endif

	if (FAILED(D3DCompileFromFile(strFilePath.c_str()
		, nullptr
		, D3D_COMPILE_STANDARD_FILE_INCLUDE
		, _FuncName.c_str(), "gs_5_0", Flag, 0
		, m_HSBlob.GetAddressOf()
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
			MessageBoxA(nullptr, "쉐이더 파일 경로가 올바르지 않습니다.", "쉐이더 컴파일 실패", MB_OK);
		}
		return E_FAIL;
	}

	if (FAILED(DEVICE->CreateHullShader(m_HSBlob->GetBufferPointer()
		, m_HSBlob->GetBufferSize()
		, nullptr
		, m_HS.GetAddressOf())))
	{
		return E_FAIL;
	}

	m_ShaderName[HULL_SHADER] = make_pair(_RelativePath, _FuncName);

	return S_OK;
}

int CGraphicShader::CreatePixelShader(const wstring& _RelativePath, const string& _FuncName)
{
	wstring strFilePath = CPathMgr::GetContentDir() + _RelativePath;

	UINT Flag = 0;

#ifdef _DEBUG
	Flag = D3DCOMPILE_DEBUG;
#endif

	if (FAILED(D3DCompileFromFile(strFilePath.c_str()
		, nullptr
		, D3D_COMPILE_STANDARD_FILE_INCLUDE
		, _FuncName.c_str(), "ps_5_0", Flag, 0
		, m_PSBlob.GetAddressOf()
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
			MessageBoxA(nullptr, "쉐이더 파일 경로가 올바르지 않습니다.", "쉐이더 컴파일 실패", MB_OK);
		}
		return E_FAIL;
	}

	if (FAILED(DEVICE->CreatePixelShader(m_PSBlob->GetBufferPointer()
		, m_PSBlob->GetBufferSize()
		, nullptr
		, m_PS.GetAddressOf())))
	{
		return E_FAIL;
	}

	m_ShaderName[PIXEL_SHADER] = make_pair(_RelativePath, _FuncName);

	return S_OK;
}

void CGraphicShader::Binding()
{
	CONTEXT->IASetInputLayout(m_Layout.Get());
	CONTEXT->IASetPrimitiveTopology(m_Topology);

	CONTEXT->VSSetShader(m_VS.Get(), nullptr, 0);
	CONTEXT->GSSetShader(m_GS.Get(), nullptr, 0);
	CONTEXT->DSSetShader(m_DS.Get(), nullptr, 0);
	CONTEXT->HSSetShader(m_HS.Get(), nullptr, 0);
	CONTEXT->PSSetShader(m_PS.Get(), nullptr, 0);

	CONTEXT->RSSetState(CDevice::GetInst()->GetRSState(m_RSType).Get());
	CONTEXT->OMSetDepthStencilState(CDevice::GetInst()->GetDSState(m_DSType).Get(), 0);
	CONTEXT->OMSetBlendState(CDevice::GetInst()->GetBSState(m_BSType).Get(), nullptr, 0xffffffff);

}

int CGraphicShader::Save(const wstring& _FileName, bool _Update)
{
	std::filesystem::path path = CPathMgr::GetContentDir() + std::wstring(L"GraphicShader\\") + _FileName + std::wstring(L".shader");
	CPathMgr::CreateParentDir(path);
	std::fstream file(path, std::ios::out | std::ios::binary);

	// hlsl 파일 경로 및 셰이더 함수 이름 정보 저장
	for (int i = 0; i < SHADER_TYPE_END; ++i)
	{
		int size = m_ShaderName[i].first.size();
		file.write(reinterpret_cast<char*>(&size), sizeof(int));
		file.write(reinterpret_cast<char*>(m_ShaderName[i].first.data()), sizeof(wchar_t) * m_ShaderName[i].first.size());
		size = m_ShaderName[i].second.size();
		file.write(reinterpret_cast<char*>(&size), sizeof(int));
		file.write(reinterpret_cast<char*>(m_ShaderName[i].second.data()), sizeof(char) * m_ShaderName[i].second.size());
	}

	// 토폴로지, 셰이더 도메인, RS, DS, BS 타입 정보 저장
	file.write(reinterpret_cast<char*>(&m_Topology), sizeof(D3D11_PRIMITIVE_TOPOLOGY));
	file.write(reinterpret_cast<char*>(&m_Domain), sizeof(SHADER_DOMAIN));
	file.write(reinterpret_cast<char*>(&m_RSType), sizeof(RS_TYPE));
	file.write(reinterpret_cast<char*>(&m_DSType), sizeof(DS_TYPE));
	file.write(reinterpret_cast<char*>(&m_BSType), sizeof(BS_TYPE));

	// 상수, 텍스쳐 데이터 저장
	int size = m_vecConstData.size();
	file.write(reinterpret_cast<char*>(&size), sizeof(int));
	for (int i = 0; i < size; ++i)
		file.write(reinterpret_cast<char*>(&m_vecConstData[i]), sizeof(tConstData));
	size = m_vecTexData.size();
	file.write(reinterpret_cast<char*>(&size), sizeof(int));
	for (int i = 0; i < size; ++i)
		file.write(reinterpret_cast<char*>(&m_vecTexData[i]), sizeof(tTexData));

	file.close();
	return S_OK;
}

int CGraphicShader::Load(const wstring& _FilePath)
{
	std::filesystem::path path = CPathMgr::GetContentDir() + _FilePath;
	std::fstream file(path, std::ios::in | std::ios::binary);
	if (!file.is_open())
		return E_FAIL;

	// hlsl 파일 경로 및 셰이더 함수 이름 정보 불러오기
	for (int i = 0; i < SHADER_TYPE_END; ++i)
	{
		int size = 0;
		file.read(reinterpret_cast<char*>(&size), sizeof(int));
		m_ShaderName[i].first.resize(size);
		file.read(reinterpret_cast<char*>(m_ShaderName[i].first.data()), sizeof(wchar_t) * size);
		file.read(reinterpret_cast<char*>(&size), sizeof(int));
		m_ShaderName[i].second.resize(size);
		file.read(reinterpret_cast<char*>(m_ShaderName[i].second.data()), sizeof(char) * size);
	}

	// 토폴로지, 셰이더 도메인, RS, DS, BS 타입 정보 불러오기
	file.read(reinterpret_cast<char*>(&m_Topology), sizeof(D3D11_PRIMITIVE_TOPOLOGY));
	file.read(reinterpret_cast<char*>(&m_Domain), sizeof(SHADER_DOMAIN));
	file.read(reinterpret_cast<char*>(&m_RSType), sizeof(RS_TYPE));
	file.read(reinterpret_cast<char*>(&m_DSType), sizeof(DS_TYPE));
	file.read(reinterpret_cast<char*>(&m_BSType), sizeof(BS_TYPE));

	// 상수, 텍스쳐 데이터 불러오기
	int size = 0;
	file.read(reinterpret_cast<char*>(&size), sizeof(int));
	m_vecConstData.resize(size);
	for (int i = 0; i < size; ++i)
		file.read(reinterpret_cast<char*>(&m_vecConstData[i]), sizeof(tConstData));
	file.read(reinterpret_cast<char*>(&size), sizeof(int));
	m_vecTexData.resize(size);
	for (int i = 0; i < size; ++i)
		file.read(reinterpret_cast<char*>(&m_vecTexData[i]), sizeof(tTexData));

	file.close();


	// hlsl 파일 경로 및 셰이더 함수 이름 정보로 각 셰이더 생성 및 컴파일
	for (int i = 0; i < SHADER_TYPE_END; ++i)
	{
		if (m_ShaderName[i].first.empty())
			continue;
		switch (i)
		{
		case VERTEX_SHADER:
			CreateVertexShader(m_ShaderName[i].first, m_ShaderName[i].second);
			break;
		case GEOMETRY_SHADER:
			CreateGeometryShader(m_ShaderName[i].first, m_ShaderName[i].second);
			break;
		case PIXEL_SHADER:
			CreatePixelShader(m_ShaderName[i].first, m_ShaderName[i].second);
			break;
		}
	}

	return S_OK;
}
