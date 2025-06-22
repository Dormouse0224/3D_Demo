#include "pch.h"
#include "CTimeMgr.h"

#include "CLevelMgr.h"
#include "CLevel.h"

#include "CEngine.h"

CTimeMgr::CTimeMgr()
	: m_Frequency{}
	, m_PrevCount{}
	, m_CurCount{}
	, m_LocalTime()
	, m_FPS(0)
	, m_DT(0.f)
	, m_Time(0.f)
	, m_EngineDT(0.f)
	, m_EngineTime(0.f)
{
}

CTimeMgr::~CTimeMgr()
{

}

void CTimeMgr::Init()
{
	// 1초에 1000 카운팅 -> 초당 카운팅 양이 너무 적음
	//GetTickCount();

	// 프레임과 프레임 사이의 시간
	QueryPerformanceFrequency(&m_Frequency); // 초당 카운팅 값 (천만)
	QueryPerformanceCounter(&m_PrevCount);
}

void CTimeMgr::Tick()
{
	// 로컬 시간 기록
	GetLocalTime(&m_LocalTime);

	// 현재시점 Count 값
	QueryPerformanceCounter(&m_CurCount);

	m_EngineDT = m_DT = (float)(m_CurCount.QuadPart - m_PrevCount.QuadPart) / (float)m_Frequency.QuadPart;

	m_PrevCount = m_CurCount;

	++m_FPS;

	// Level State 에 따라 DT 활성화/비활성화, m_Time 은 DT 누적시간이기 때문에 STOP 일 경우 누적되지 않음
	LEVEL_STATE State = LEVEL_STATE::STOP;
	if (CLevelMgr::GetInst()->GetCurrentLevel())
		State = CLevelMgr::GetInst()->GetCurrentLevel()->GetState();

	if (LEVEL_STATE::PLAY != State)
	{
		m_DT = 0.f;
	}
	else
	{
		m_Time += m_DT;
	}

	m_EngineTime += m_EngineDT;

	static float OneSecond = 0.f;
	OneSecond += m_EngineDT;
	if (1.f <= OneSecond)
	{
		wchar_t buff[255] = {};
		swprintf_s(buff, 255, L"FPS : %d, DeltaTime : %f", m_FPS, m_DT);
		SetWindowText(CEngine::GetInst()->GetMainWndHwnd(), buff);

		m_FPS = 0;
		OneSecond -= 1.f;
	}

	// 전역 상수 데이터 갱신
	g_global.g_DT = m_DT;
	g_global.g_EngineDT = m_EngineDT;
	g_global.g_Time = m_Time;
	g_global.g_EngineTime = m_EngineTime;
}
