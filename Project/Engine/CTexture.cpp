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

int CTexture::Create(UINT _Width, UINT _Height, DXGI_FORMAT _format
	, UINT _Flag, D3D11_USAGE _usage, int _TexArrCount, bool _IsCube)
{
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

