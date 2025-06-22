#pragma once

class CConstBuffer;
class CTexture2D;

class CDevice
	: public Singleton<CDevice>
{
	SINGLE(CDevice);
private:
	HWND								m_hMainWnd;
	D3D11_VIEWPORT						m_ViewPort;
	Vec2								m_RenderResolution;			// 렌더타겟 해상도

	ComPtr<ID3D11Device>				m_Device;					// GPU 메모리 제어, DirectX 객체 생성
	ComPtr<ID3D11DeviceContext>			m_Context;					// GPU 명령 수행

	ComPtr<IDXGISwapChain>				m_SwapChain;				// Window 화면에 렌더링

	CConstBuffer*						m_ConstBuffer[(UINT)CB_TYPE::END];

	ComPtr<ID3D11RasterizerState>		m_RSState[(UINT)RS_TYPE::END];
	ComPtr<ID3D11DepthStencilState>		m_DSState[(UINT)DS_TYPE::END];
	ComPtr<ID3D11BlendState>			m_BSState[(UINT)BS_TYPE::END];

	ComPtr<ID3D11SamplerState>			m_Sampler[2];			// 샘플링(색상 추출)



public:
	int Init(HWND _OutputWnd, Vec2 _vResolution);
	void Present();

	ComPtr<ID3D11Device> GetDevice() { return m_Device; }
	ComPtr<ID3D11DeviceContext> GetContext() { return m_Context; }
	ComPtr<IDXGISwapChain> GetSwapChain() { return m_SwapChain; }
	D3D11_VIEWPORT GetViewPort() { return m_ViewPort; }
	CConstBuffer* GetConstBuffer(CB_TYPE _Type) { return m_ConstBuffer[(UINT)_Type]; }
	Vec2 GetRenderResolution() { return m_RenderResolution; }
	ComPtr<ID3D11RasterizerState> GetRSState(RS_TYPE _Type) { return m_RSState[(UINT)_Type]; }
	ComPtr<ID3D11DepthStencilState> GetDSState(DS_TYPE _Type) { return m_DSState[(UINT)_Type]; }
	ComPtr<ID3D11BlendState>	GetBSState(BS_TYPE _Type) { return m_BSState[(UINT)_Type]; }

private:
	int CreateSwapChain();
	void CreateConstBuffer();
	void CreateRasterizerState();
	void CreateDepthStencilState();
	void CreateBlendState();
	void CreateSamplerState();
};

