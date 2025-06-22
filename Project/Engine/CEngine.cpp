#include "pch.h"
#include "CEngine.h"

#include "CDevice.h"
#include "CPathMgr.h"
#include "CKeyMgr.h"
#include "CTimeMgr.h"
#include "CAssetMgr.h"
#include "CLevelMgr.h"
#include "CRenderMgr.h"
#include "CTaskMgr.h"
#include "CPhysxMgr.h"
#include "CImguiMgr.h"
#include "CLightMgr.h"

CEngine::CEngine()
    : m_hInst(nullptr)
    , m_hMainWnd(nullptr)
    , m_FMODSystem(nullptr)
{

}

CEngine::~CEngine()
{
    if (nullptr != m_FMODSystem)
    {
        m_FMODSystem->release();
        m_FMODSystem = nullptr;
    }

    LiveObjectReport();
}

int CEngine::Init(HINSTANCE _Inst, UINT _Width, UINT _Height)
{
    m_hInst = _Inst;
    m_vResolution = Vec2(_Width, _Height);

    // enum extern 컨테이너 일치 확인
    enumSizeCheck();

    if (FAILED(CreateMainWindow()))
    {
        return E_FAIL;
    }

    // Device 초기화
    if (FAILED(CDevice::GetInst()->Init(m_hMainWnd, m_vResolution)))
    {
        return E_FAIL;
    }

    // FMOD 초기화 및 32개 채널 생성
    FMOD::System_Create(&m_FMODSystem);
    assert(m_FMODSystem);
    m_FMODSystem->init(32, FMOD_DEFAULT, nullptr);

    // Manager 초기화
    CPathMgr::Init();
    CTimeMgr::GetInst()->Init();
    CPhysxMgr::GetInst()->Init();
    CAssetMgr::GetInst()->Init();
    CKeyMgr::GetInst()->Init();
    CLevelMgr::GetInst()->Init();
    CRenderMgr::GetInst()->Init();
    CImguiMgr::GetInst()->Init();

    return S_OK;
}

void CEngine::Begin()
{
    // 스크립트, FSM 등록까지 완료되면 콘텐츠 파일들을 로드한다. (프리펩 로딩에서 스크립트가 필요한 경우가 있음)
    CAssetMgr::GetInst()->ContentLoad();
}

void CEngine::Progress()
{
    // Manager Tick
    CTimeMgr::GetInst()->Tick();
    CKeyMgr::GetInst()->Tick();
    CAssetMgr::GetInst()->Tick();
    CLightMgr::GetInst()->Tick();

    // Level Progress
    CLevelMgr::GetInst()->Progress();

    // physx simulate
    CPhysxMgr::GetInst()->Tick();

    // Imgui progress & Render additional window
    CImguiMgr::GetInst()->Progress();

    // Render
    CRenderMgr::GetInst()->Tick();

    // Task
    CTaskMgr::GetInst()->Tick();
}

int CEngine::CreateMainWindow()
{
    m_hMainWnd = CreateWindowW(L"ClientWindow", L"SNB", WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, m_hInst, nullptr);

    if (!m_hMainWnd)
        return E_FAIL;

    ShowWindow(m_hMainWnd, true);
    UpdateWindow(m_hMainWnd);

    // 입력된 해상도에 맞는 윈도우 크기 설정
    RECT rt = { 0, 0, (int)m_vResolution.x, (int)m_vResolution.y };
    AdjustWindowRect(&rt, WS_OVERLAPPEDWINDOW, !!GetMenu(m_hMainWnd));
    SetWindowPos(m_hMainWnd, nullptr, 0, 0, rt.right - rt.left, rt.bottom - rt.top, 0);

    GetClientRect(m_hMainWnd, &rt);
    m_vResolution = Vec2(static_cast<float>(rt.right - rt.left), static_cast<float>(rt.bottom - rt.top));
    g_global.g_Resolution = m_vResolution;

    return S_OK;
}

void CEngine::LiveObjectReport()
{
#ifndef _DEBUG
    return;
#endif

    HMODULE dxgidebugdll = GetModuleHandleW(L"dxgidebug.dll");
    decltype(&DXGIGetDebugInterface) GetDebugInterface = reinterpret_cast<decltype(&DXGIGetDebugInterface)>(GetProcAddress(dxgidebugdll, "DXGIGetDebugInterface"));

    IDXGIDebug* debug;

    GetDebugInterface(IID_PPV_ARGS(&debug));

    OutputDebugStringW(L"Starting Live Direct3D Object Dump:\r\n");
    debug->ReportLiveObjects(DXGI_DEBUG_D3D11, DXGI_DEBUG_RLO_DETAIL);
    OutputDebugStringW(L"Completed Live Direct3D Object Dump.\r\n");

    debug->Release();
}