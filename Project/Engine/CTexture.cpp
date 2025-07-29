#include "pch.h"
#include "CTexture.h"

#include "CPathMgr.h"

#include "CDevice.h"

CTexture::CTexture()
	: CAsset(ASSET_TYPE::TEXTURE)
{
}

CTexture::~CTexture()
{
}

int CTexture::Save(const wstring& _FileName, bool _Update)
{
    wstring SaveDir = CPathMgr::GetContentDir();
    if (_Update)
    {
        // 버전 업데이트 신규 파일인 경우 업데이트 디렉토리에 저장
        std::filesystem::path parentDir = SaveDir;
        SaveDir = parentDir.parent_path().parent_path();
        SaveDir += L"\\Update\\";
    }
    std::filesystem::path path = SaveDir + std::wstring(L"Texture\\") + _FileName + std::wstring(L".dds");
    CPathMgr::CreateParentDir(path);
    if (FAILED(SaveDDSTextureToFile(CONTEXT, m_Tex2D.Get(), path.c_str())))
        return E_FAIL;

    return S_OK;
}

int CTexture::Load(const wstring& _FilePath)
{
	/*wchar_t szExt[50] = {};
	_wsplitpath_s(_FilePath.c_str(), nullptr, 0, nullptr, 0, nullptr, 0, szExt, 50);*/

	std::filesystem::path fullpath = CPathMgr::GetContentDir() + _FilePath;

	wstring EXT = fullpath.extension();

	// .dds .DDS
	if (EXT == L".dds" || EXT == L".DDS")
	{
		if (FAILED(LoadFromDDSFile(fullpath.c_str(), DDS_FLAGS_NONE, nullptr, m_Image)))
		{
			return E_FAIL;
		}
	}
	// .tga .TGA
	else if (EXT == L".tga" || EXT == L".TGA")
	{
		if (FAILED(LoadFromTGAFile(fullpath.c_str(), nullptr, m_Image)))
		{
			return E_FAIL;
		}
	}

	// WIC : png, jpg, jpeg, bmp
	else if (EXT == L".png" || EXT == L".PNG"
		|| EXT == L".jpg" || EXT == L".JPG"
		|| EXT == L".jpeg" || EXT == L".JPEG"
		|| EXT == L".bmp" || EXT == L".BMP")
	{
		if (FAILED(LoadFromWICFile(fullpath.c_str(), WIC_FLAGS_NONE, nullptr, m_Image)))
		{
			return E_FAIL;
		}
	}

	else
	{
		return E_FAIL;
	}

	// Image -> Tex2D -> SRV 생성
	CreateShaderResourceView(DEVICE
		, m_Image.GetImages()
		, m_Image.GetImageCount()
		, m_Image.GetMetadata()
		, m_SRV.GetAddressOf());

	// SRV 로 Tex2D 주소를 알아낸다.
	m_SRV->GetResource((ID3D11Resource**)m_Tex2D.GetAddressOf());

	// Desc 정보 받아오기
	m_Tex2D->GetDesc(&m_Desc);

	DEBUG_NAME(m_Tex2D, "CTexture_Tex2D");

	return S_OK;
}

void CTexture::Binding(UINT _RegisterNum)
{
    CONTEXT->VSSetShaderResources(_RegisterNum, 1, m_SRV.GetAddressOf());
    CONTEXT->HSSetShaderResources(_RegisterNum, 1, m_SRV.GetAddressOf());
    CONTEXT->DSSetShaderResources(_RegisterNum, 1, m_SRV.GetAddressOf());
    CONTEXT->GSSetShaderResources(_RegisterNum, 1, m_SRV.GetAddressOf());
    CONTEXT->PSSetShaderResources(_RegisterNum, 1, m_SRV.GetAddressOf());
}

void CTexture::Unbind(UINT _RegisterNum)
{
    ID3D11ShaderResourceView* nullSRV = nullptr;

    CONTEXT->VSSetShaderResources(_RegisterNum, 1, &nullSRV);
    CONTEXT->HSSetShaderResources(_RegisterNum, 1, &nullSRV);
    CONTEXT->DSSetShaderResources(_RegisterNum, 1, &nullSRV);
    CONTEXT->GSSetShaderResources(_RegisterNum, 1, &nullSRV);
    CONTEXT->PSSetShaderResources(_RegisterNum, 1, &nullSRV);
}

void CTexture::Binding_CS(UINT _RegisterNum)
{
	m_RecentSRV_CS = _RegisterNum;
	CONTEXT->CSSetShaderResources(_RegisterNum, 1, m_SRV.GetAddressOf());
}

void CTexture::Binding_CS_UAV(UINT _RegisterNum)
{
	m_RecentUAV_CS = _RegisterNum;
	UINT i = -1;
	CONTEXT->CSSetUnorderedAccessViews(_RegisterNum, 1, m_UAV.GetAddressOf(), &i);
}

void CTexture::Unbind_CS()
{
    ID3D11ShaderResourceView* pSRV = nullptr;
    CONTEXT->CSSetShaderResources(m_RecentSRV_CS, 1, &pSRV);
    m_RecentSRV_CS = -1;
}

void CTexture::Unbind_CS_UAV()
{
    ID3D11UnorderedAccessView* pUAV = nullptr;
    UINT i = -1;
    CONTEXT->CSSetUnorderedAccessViews(m_RecentUAV_CS, 1, &pUAV, &i);
    m_RecentUAV_CS = -1;
}

int CTexture::CreateUAV()
{
    if (m_Desc.BindFlags & D3D11_BIND_DEPTH_STENCIL || m_Desc.BindFlags & D3D11_BIND_UNORDERED_ACCESS || m_UAV != nullptr)
        return E_FAIL;

    m_Desc.BindFlags |= D3D11_BIND_UNORDERED_ACCESS;
    ComPtr<ID3D11Texture2D> pNewTex = nullptr;
    DEVICE->CreateTexture2D(&m_Desc, nullptr, pNewTex.GetAddressOf());
    CONTEXT->CopyResource(pNewTex.Get(), m_Tex2D.Get());

    if (FAILED(Create(pNewTex)))
        return E_FAIL;

    return S_OK;
}

int CTexture::Create(UINT _Width, UINT _Height, DXGI_FORMAT _format
	, UINT _Flag, D3D11_USAGE _usage, int _TexArrCount, bool _IsCube)
{
    m_Tex2D = nullptr;
    m_RTV = nullptr;
    m_DSV = nullptr;
    m_SRV = nullptr;
    m_UAV = nullptr;

	m_Desc.Format = _format;
	m_Desc.ArraySize = _TexArrCount;
    m_Desc.MiscFlags = 0;
	m_Desc.Width = _Width;
	m_Desc.Height = _Height;
	m_Desc.BindFlags = _Flag;

	// System Memroy 이동 불가능
	m_Desc.Usage = _usage;

	if (m_Desc.Usage == D3D11_USAGE::D3D11_USAGE_DYNAMIC)
		m_Desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	else
		m_Desc.CPUAccessFlags = 0;

	m_Desc.MipLevels = 1;
	m_Desc.SampleDesc.Count = 1;
	m_Desc.SampleDesc.Quality = 0;

    if (_IsCube)
    {
        m_Desc.ArraySize = 6;
        m_Desc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;
        m_Desc.Height = m_Desc.Width;
    }

	if (FAILED(DEVICE->CreateTexture2D(&m_Desc, nullptr, m_Tex2D.GetAddressOf())))
	{
		return E_FAIL;
	}


	if (m_Desc.BindFlags & D3D11_BIND_DEPTH_STENCIL)
	{
		if (FAILED(DEVICE->CreateDepthStencilView(m_Tex2D.Get(), nullptr, m_DSV.GetAddressOf())))
		{
			return E_FAIL;
		}
	}

	else
	{
		if (m_Desc.BindFlags & D3D11_BIND_RENDER_TARGET)
		{
			if (FAILED(DEVICE->CreateRenderTargetView(m_Tex2D.Get(), nullptr, m_RTV.GetAddressOf())))
			{
				return E_FAIL;
			}
		}

		if (m_Desc.BindFlags & D3D11_BIND_SHADER_RESOURCE)
		{
			if (FAILED(DEVICE->CreateShaderResourceView(m_Tex2D.Get(), nullptr, m_SRV.GetAddressOf())))
			{
				return E_FAIL;
			}
		}

		if (m_Desc.BindFlags & D3D11_BIND_UNORDERED_ACCESS)
		{
			if (FAILED(DEVICE->CreateUnorderedAccessView(m_Tex2D.Get(), nullptr, m_UAV.GetAddressOf())))
			{
				return E_FAIL;
			}
		}
	}

}

int CTexture::Create(ComPtr<ID3D11Texture2D> _Tex2D)
{
    m_Tex2D = nullptr;
    m_RTV = nullptr;
    m_DSV = nullptr;
    m_SRV = nullptr;
    m_UAV = nullptr;

	m_Tex2D = _Tex2D;

	_Tex2D->GetDesc(&m_Desc);

	if (m_Desc.BindFlags & D3D11_BIND_DEPTH_STENCIL)
	{
		if (FAILED(DEVICE->CreateDepthStencilView(m_Tex2D.Get(), nullptr, m_DSV.GetAddressOf())))
		{
			return E_FAIL;
		}
	}

	else
	{
		if (m_Desc.BindFlags & D3D11_BIND_RENDER_TARGET)
		{
			if (FAILED(DEVICE->CreateRenderTargetView(m_Tex2D.Get(), nullptr, m_RTV.GetAddressOf())))
			{
				return E_FAIL;
			}
		}

		if (m_Desc.BindFlags & D3D11_BIND_SHADER_RESOURCE)
		{
			if (FAILED(DEVICE->CreateShaderResourceView(m_Tex2D.Get(), nullptr, m_SRV.GetAddressOf())))
			{
				return E_FAIL;
			}
		}

		if (m_Desc.BindFlags & D3D11_BIND_UNORDERED_ACCESS)
		{
			if (FAILED(DEVICE->CreateUnorderedAccessView(m_Tex2D.Get(), nullptr, m_UAV.GetAddressOf())))
			{
				return E_FAIL;
			}
		}
	}

}
int CTexture::GenerateMip(UINT _Level)
{
    // CubeTexture 는 Mipmap 생성 금지
    //assert(false == m_Desc.MiscFlags & D3D11_SRV_DIMENSION_TEXTURECUBE);

    if (m_RTV.Get() || m_UAV.Get())
        return E_FAIL;

    m_Tex2D = nullptr;
    m_RTV = nullptr;
    m_DSV = nullptr;
    m_SRV = nullptr;
    m_UAV = nullptr;

    m_Desc.MipLevels = _Level;
    m_Desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
    m_Desc.MiscFlags |= D3D11_RESOURCE_MISC_GENERATE_MIPS;

    if (FAILED(DEVICE->CreateTexture2D(&m_Desc, nullptr, m_Tex2D.GetAddressOf())))
    {
        return E_FAIL;
    }

    for (UINT i = 0; i < m_Desc.ArraySize; ++i)
    {
        UINT iSubIdx = D3D11CalcSubresource(0, i, m_Desc.MipLevels);

        CONTEXT->UpdateSubresource(m_Tex2D.Get(), iSubIdx, nullptr
            , m_Image.GetImage(0, i, 0)->pixels
            , m_Image.GetImage(0, i, 0)->rowPitch
            , m_Image.GetImage(0, i, 0)->slicePitch);
    }

    D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc = {};

    if (2 <= m_Desc.ArraySize)
    {
        SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
        SRVDesc.Texture2DArray.ArraySize = m_Desc.ArraySize;
        SRVDesc.Texture2DArray.MipLevels = m_Desc.MipLevels;
        SRVDesc.Texture2DArray.MostDetailedMip = 0;
    }
    else
    {
        SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
        SRVDesc.Texture2D.MipLevels = m_Desc.MipLevels;
        SRVDesc.Texture2D.MostDetailedMip = 0;
    }

    if (FAILED(DEVICE->CreateShaderResourceView(m_Tex2D.Get(), &SRVDesc, m_SRV.GetAddressOf())))
        return E_FAIL;

    CONTEXT->GenerateMips(m_SRV.Get());

    return S_OK;
}