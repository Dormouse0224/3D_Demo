#pragma once

class CEngine
	: public Singleton<CEngine>
{
	SINGLE(CEngine);
private:
	HINSTANCE	m_hInst;		// 프로세스 주소
	HWND		m_hMainWnd;		// 메인 윈도우 핸들
	Vec2		m_vResolution;	// 메인 윈도우 해상도

	// FMOD 관리자 클래스
	FMOD::System* m_FMODSystem;	// FMOD 최상위 관리자 객체

public:
	int Init(HINSTANCE _Inst, UINT _Width, UINT _Height);
	void Begin();
	void Progress();

	Vec2 GetResolution() { return m_vResolution; }
	HWND GetMainWndHwnd() { return m_hMainWnd; }
	FMOD::System* GetFMODSystem() { return m_FMODSystem; }

private:
	void LiveObjectReport();
	int CreateMainWindow();
};

