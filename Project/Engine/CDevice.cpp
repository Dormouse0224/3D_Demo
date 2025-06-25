#include "pch.h"
#include "CDevice.h"

#include "CConstBuffer.h"
#include "CTexture.h"

#include "CLevelMgr.h"
#include "CAssetMgr.h"


CDevice::CDevice()
    : m_hMainWnd(nullptr)
    , m_ConstBuffer{}
    , m_RSState{}
    , m_ViewPort{}
{

}

CDevice::~CDevice()
{
    for (int i = 0; i < (UINT)CB_TYPE::END; ++i)
    {
        if (nullptr != m_ConstBuffer[i])
            delete m_ConstBuffer[i];
    }

    // 전체화면 해제
    m_SwapChain->SetFullscreenState(false, nullptr);
}

int CDevice::Init(HWND _OutputWnd, Vec2 _vResolution)
{
    m_hMainWnd = _OutputWnd;
    m_RenderResolution = _vResolution;

    UINT Flag = 0;
#ifdef _DEBUG
    Flag = D3D11_CREATE_DEVICE_DEBUG;
#endif
    D3D_FEATURE_LEVEL level = D3D_FEATURE_LEVEL_11_0;

    if (FAILED(D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr
        , Flag, 0, 0, D3D11_SDK_VERSION
        , m_Device.GetAddressOf(), &level, m_Context.GetAddressOf())))
    {
        MessageBox(nullptr, L"Device, Context 생성 실패", L"Device 초기화 오류", MB_OK);
        return E_FAIL;
    }

    // 스왚체인 생성
    if (FAILED(CreateSwapChain()))
    {
        MessageBox(nullptr, L"SwapChain 생성 실패", L"Device 초기화 오류", MB_OK);
        return E_FAIL;
    }

    // RaterizerState 종류별로 생성
    CreateRasterizerState();

    // DepthStencilState 생성
    CreateDepthStencilState();

    // BlendState 생성
    CreateBlendState();

    // 상수버퍼를 각 상수 레지스터에 대응할 수 있도록 하나씩 만들어 둠
    CreateConstBuffer();

    // ViewPort 설정
    m_ViewPort.TopLeftX = 0;
    m_ViewPort.TopLeftY = 0;
    m_ViewPort.Width = m_RenderResolution.x;
    m_ViewPort.Height = m_RenderResolution.y;
    m_ViewPort.MinDepth = 0.f;    // 깊이 텍스쳐에 저장하는 깊이값의 범위
    m_ViewPort.MaxDepth = 1.f;

    // 샘플러 생성 및 바인딩
    CreateSamplerState();


    return S_OK;
}

void CDevice::Present()
{
    m_SwapChain->Present(0, 0);
}

int CDevice::CreateSwapChain()
{
    DXGI_SWAP_CHAIN_DESC Desc = {};

    Desc.OutputWindow = m_hMainWnd;   // 생성된 스왚체인이 이미지를 출력시킬 목적지 윈도우
    Desc.Windowed = true;             // 윈도우 모드, 전체화면 모드

    Desc.Flags = 0;
    Desc.SampleDesc.Count = 1;
    Desc.SampleDesc.Quality = 0;
    Desc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    Desc.BufferCount = 1;             // 출력 Buffer 개수         
    Desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT; // Buffer 용도
    Desc.BufferDesc.Width = (UINT)m_RenderResolution.x;
    Desc.BufferDesc.Height = (UINT)m_RenderResolution.y;
    Desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    Desc.BufferDesc.RefreshRate.Numerator = 60;
    Desc.BufferDesc.RefreshRate.Denominator = 1;
    Desc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    Desc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

    ComPtr<IDXGIDevice> pDevice = nullptr;
    ComPtr<IDXGIAdapter> pAdapter = nullptr;
    ComPtr<IDXGIFactory> pFactory = nullptr;

    // 디바이스로부터 어댑터를 거쳐 팩토리를 얻어낸다.
    // DXGI 기반 작업에서는 DXGI 디바이스가 필요하므로 기존 D3D11 디바이스에서 얻어온다.
    m_Device->QueryInterface(__uuidof(IDXGIDevice), (void**)&pDevice);
    pDevice->GetParent(__uuidof(IDXGIAdapter), (void**)&pAdapter);
    pAdapter->GetParent(__uuidof(IDXGIFactory), (void**)&pFactory);

    if (FAILED(pFactory->CreateSwapChain(pDevice.Get(), &Desc, m_SwapChain.GetAddressOf())))
    {
        return E_FAIL;
    }

    return S_OK;
}

void CDevice::CreateConstBuffer()
{
    m_ConstBuffer[(UINT)CB_TYPE::TRANSFORM] = new CConstBuffer;
    m_ConstBuffer[(UINT)CB_TYPE::TRANSFORM]->Create(CB_TYPE::TRANSFORM, sizeof(tTransform));

    m_ConstBuffer[(UINT)CB_TYPE::MATERIAL] = new CConstBuffer;
    m_ConstBuffer[(UINT)CB_TYPE::MATERIAL]->Create(CB_TYPE::MATERIAL, sizeof(tMtrlConst));

    m_ConstBuffer[(UINT)CB_TYPE::GLOBAL] = new CConstBuffer;
    m_ConstBuffer[(UINT)CB_TYPE::GLOBAL]->Create(CB_TYPE::GLOBAL, sizeof(tGlobal));
}

void CDevice::CreateRasterizerState()
{
    m_RSState[(UINT)RS_TYPE::CULL_BACK] = nullptr;

    D3D11_RASTERIZER_DESC Desc = {};
    Desc.CullMode = D3D11_CULL_FRONT;
    Desc.FillMode = D3D11_FILL_SOLID;
    DEVICE->CreateRasterizerState(&Desc, m_RSState[(UINT)RS_TYPE::CULL_FRONT].GetAddressOf());

    Desc.CullMode = D3D11_CULL_NONE;
    Desc.FillMode = D3D11_FILL_SOLID;
    DEVICE->CreateRasterizerState(&Desc, m_RSState[(UINT)RS_TYPE::CULL_NONE].GetAddressOf());

    Desc.CullMode = D3D11_CULL_NONE;
    Desc.FillMode = D3D11_FILL_WIREFRAME;
    DEVICE->CreateRasterizerState(&Desc, m_RSState[(UINT)RS_TYPE::WIRE_FRAME].GetAddressOf());
}

void CDevice::CreateDepthStencilState()
{
    m_DSState[(UINT)DS_TYPE::LESS] = nullptr;

    D3D11_DEPTH_STENCIL_DESC Desc = {};

    // Less Equal
    Desc.DepthEnable = true;
    Desc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
    Desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL; // 깊이 기록 O
    Desc.StencilEnable = false;
    DEVICE->CreateDepthStencilState(&Desc, m_DSState[(UINT)DS_TYPE::LESS_EQUAL].GetAddressOf());

    // Greater
    Desc.DepthEnable = true;
    Desc.DepthFunc = D3D11_COMPARISON_GREATER;
    Desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL; // 깊이 기록 O
    Desc.StencilEnable = false;
    DEVICE->CreateDepthStencilState(&Desc, m_DSState[(UINT)DS_TYPE::GREATER].GetAddressOf());

    // NoWrite
    Desc.DepthEnable = true;
    Desc.DepthFunc = D3D11_COMPARISON_LESS;
    Desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO; // 깊이 기록 X
    Desc.StencilEnable = false;
    DEVICE->CreateDepthStencilState(&Desc, m_DSState[(UINT)DS_TYPE::NO_WRITE].GetAddressOf());

    // NoTest NoWrite 
    Desc.DepthEnable = true;
    Desc.DepthFunc = D3D11_COMPARISON_ALWAYS;          // 깊이 항상 통과(검사 X)
    Desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO; // 깊이 기록 X
    Desc.StencilEnable = false;
    DEVICE->CreateDepthStencilState(&Desc, m_DSState[(UINT)DS_TYPE::NO_TEST_NO_WIRITE].GetAddressOf());

    // volume mesh check
    Desc.DepthEnable = true;
    Desc.DepthFunc = D3D11_COMPARISON_GREATER;
    Desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO; // 깊이 기록 X

    Desc.StencilEnable = true;
    Desc.StencilReadMask = 0xff;
    Desc.StencilWriteMask = 0xff;

    // 뒷면 스텐실 테스트
    Desc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;    // 스탠실 항상 통과
    Desc.BackFace.StencilPassOp = D3D11_STENCIL_OP_INCR;    // 성공하면 스텐실값을 증가
    Desc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;    // 스탠실 실패시 : 항상 통과하므로 의미 없음
    Desc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_ZERO;   // 스텐실은 통과했지만 깊이 테스트에서 실패한 경우 스텐실값을 0으로 설정

    Desc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;   // 스탠실 항상 통과
    Desc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;  // Grater 조건으로 앞면을 그린다.
    Desc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_ZERO;  // 성공한게 오히려 실패, 앞면보다 더 앞쪽에 있는 부위 
    Desc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;

    DEVICE->CreateDepthStencilState(&Desc, m_DSState[(UINT)DS_TYPE::VOLUME_MESH_CHECK].GetAddressOf());

    // stencil equal check
    // 특정 스텐실값이 있는 부분만 통과
    Desc.DepthEnable = true;
    Desc.DepthFunc = D3D11_COMPARISON_ALWAYS;
    Desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO; // 깊이 기록 X

    Desc.StencilEnable = true;
    Desc.StencilReadMask = 0xff;
    Desc.StencilWriteMask = 0xff;

    Desc.BackFace.StencilFunc = D3D11_COMPARISON_LESS; // 특정 값인경우 통과
    Desc.BackFace.StencilPassOp = D3D11_STENCIL_OP_ZERO; // 모든 스텐실 값을 0로 만든다.
    Desc.BackFace.StencilFailOp = D3D11_STENCIL_OP_ZERO;
    Desc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_ZERO;

    Desc.FrontFace.StencilFunc = D3D11_COMPARISON_LESS; // 특정 값인경우 통과
    Desc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_ZERO;  // 모든 스텐실 값을 0로 만든다.
    Desc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_ZERO;
    Desc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_ZERO;

    DEVICE->CreateDepthStencilState(&Desc, m_DSState[(UINT)DS_TYPE::STENCIL_EQUAL].GetAddressOf());
}

void CDevice::CreateBlendState()
{
    // Src : 1, Dst : 0
    m_BSState[(UINT)BS_TYPE::DEFAULT] = nullptr;


    // AlphaBlend
    D3D11_BLEND_DESC Desc = {};
    Desc.AlphaToCoverageEnable = false;
    Desc.IndependentBlendEnable = false;
    {
        // 0번 렌더 타겟 알파블렌드 설정
        Desc.RenderTarget[0].BlendEnable = true;
        Desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

        // RGB 혼합 계수 설정
        Desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;          // Src 와 Dst 를 더하기    
        Desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;      // Src 계수는 SrcAlpha
        Desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA; // Dst 계수는 (1 - SrcAlpha)

        // A 혼합 계수 설정
        Desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
        Desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
        Desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
    }

    DEVICE->CreateBlendState(&Desc, m_BSState[(UINT)BS_TYPE::ALPHABLEND].GetAddressOf());
}

void CDevice::CreateSamplerState()
{
    D3D11_SAMPLER_DESC Desc = {};

    Desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    Desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    Desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    Desc.Filter = D3D11_FILTER_ANISOTROPIC; // 이방성 필터링
    // MipMap
    Desc.MinLOD = 0;
    Desc.MaxLOD = 0;
    DEVICE->CreateSamplerState(&Desc, m_Sampler[0].GetAddressOf());


    Desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    Desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    Desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    Desc.Filter = D3D11_FILTER_MAXIMUM_MIN_MAG_MIP_POINT; // 2D 필터
    // MipMap
    Desc.MinLOD = 0;
    Desc.MaxLOD = 0;
    DEVICE->CreateSamplerState(&Desc, m_Sampler[1].GetAddressOf());

    CONTEXT->VSSetSamplers(0, 1, m_Sampler[0].GetAddressOf());
    CONTEXT->HSSetSamplers(0, 1, m_Sampler[0].GetAddressOf());
    CONTEXT->DSSetSamplers(0, 1, m_Sampler[0].GetAddressOf());
    CONTEXT->GSSetSamplers(0, 1, m_Sampler[0].GetAddressOf());
    CONTEXT->PSSetSamplers(0, 1, m_Sampler[0].GetAddressOf());
    CONTEXT->CSSetSamplers(0, 1, m_Sampler[0].GetAddressOf());

    CONTEXT->VSSetSamplers(1, 1, m_Sampler[1].GetAddressOf());
    CONTEXT->HSSetSamplers(1, 1, m_Sampler[1].GetAddressOf());
    CONTEXT->DSSetSamplers(1, 1, m_Sampler[1].GetAddressOf());
    CONTEXT->GSSetSamplers(1, 1, m_Sampler[1].GetAddressOf());
    CONTEXT->PSSetSamplers(1, 1, m_Sampler[1].GetAddressOf());
    CONTEXT->CSSetSamplers(1, 1, m_Sampler[1].GetAddressOf());
}
