#include "pch.h"
#include "CKeyMgr.h"
#include "CEngine.h"
#include "CTimeMgr.h"
#include "CAssetMgr.h"
#include "CRenderMgr.h"

#include "CGameObject.h"
#include "CTransform.h"
#include "CMeshRender.h"
#include "CCamera.h"

UINT key_value[(int)Keyboard::Keyboard_end] = 
{
    VK_OEM_3,
    VK_OEM_MINUS,
    VK_OEM_PLUS,
    VK_OEM_4,
    VK_OEM_6,
    VK_OEM_5,
    VK_OEM_1,
    VK_OEM_7,
    VK_OEM_COMMA,
    VK_OEM_PERIOD,
    VK_OEM_2,

    'Q',
    'W',
    'E',
    'R',
    'T',
    'Y',
    'U',
    'I',
    'O',
    'P',
    'A',
    'S',
    'D',
    'F',
    'G',
    'H',
    'J',
    'K',
    'L',
    'Z',
    'X',
    'C',
    'V',
    'B',
    'N',
    'M',

    '1',
    '2',
    '3',
    '4',
    '5',
    '6',
    '7',
    '8',
    '9',
    '0',

    VK_F1,
    VK_F2,
    VK_F3,
    VK_F4,
    VK_F5,
    VK_F6,
    VK_F7,
    VK_F8,
    VK_F9,
    VK_F10,
    VK_F11,
    VK_F12,

    VK_ESCAPE,
    VK_TAB,
    VK_CAPITAL,
    VK_LSHIFT,
    VK_LCONTROL,
    VK_LMENU,
    VK_BACK,
    VK_RETURN,
    VK_RSHIFT,
    VK_RMENU,
    VK_RCONTROL,
    VK_SPACE,

    VK_UP,
    VK_DOWN,
    VK_LEFT,
    VK_RIGHT,

    VK_DECIMAL,
    VK_NUMPAD0,
    VK_NUMPAD1,
    VK_NUMPAD2,
    VK_NUMPAD3,
    VK_NUMPAD4,
    VK_NUMPAD5,
    VK_NUMPAD6,
    VK_NUMPAD7,
    VK_NUMPAD8,
    VK_NUMPAD9,

    VK_LBUTTON,
    VK_RBUTTON

    // Keyboard_end
};


CKeyMgr::CKeyMgr()
    : m_MousePos(0, 0)
    , m_CommandTimeout(0)
    , m_Cursor(nullptr)
{

}

CKeyMgr::~CKeyMgr()
{
    delete m_Cursor;
}

void CKeyMgr::SetCursorTex(AssetPtr<CTexture> _Tex)
{
    if (_Tex.Get())
        m_Cursor->MeshRender()->GetMaterial()->SetTexParam(TEX_0, _Tex);
}

void CKeyMgr::Init()
{
	// 모든 키 상태를 NONE, 이전에 눌린 적 없음으로 초기화
	for (int i = 0; i < (int)Keyboard::Keyboard_end; ++i)
	{
		m_keyboardInfo.push_back(key_info{ Key_state::NONE, false });
	}


    m_Cursor = new CGameObject;
    m_Cursor->AddComponent(new CTransform);
    m_Cursor->AddComponent(new CMeshRender);
    m_Cursor->SetLayerIdx((int)LAYER::UI);
    m_Cursor->MeshRender()->SetMesh(CAssetMgr::GetInst()->Load<CMesh>(L"EA_PointMesh"));
    m_Cursor->MeshRender()->SetMaterial(CAssetMgr::GetInst()->Load<CMaterial>(L"EA_CursorMtrl", true));
    m_Cursor->MeshRender()->GetMaterial()->SetConstParam(FLOAT_1, 100.f);
    m_Cursor->MeshRender()->GetMaterial()->SetConstParam(FLOAT_2, 100.f);
}

void CKeyMgr::Tick()
{
    if (GetFocus() == CEngine::GetInst()->GetMainWndHwnd())
    {
        // 키보드 상태 업데이트
        for (int i = 0; i < (int)Keyboard::Keyboard_end; ++i)
        {
            // 키보드가 이전에 눌렸었음
            if (m_keyboardInfo[i].isPrevPressed)
            {
                // 현재도 눌린 상태임
                if (GetAsyncKeyState(key_value[i]) & 0x8001)
                {
                    m_keyboardInfo[i].state = Key_state::PRESSED;
                }
                // 현재는 떼어짐
                else
                {
                    m_keyboardInfo[i].state = Key_state::RELEASE;
                    m_keyboardInfo[i].isPrevPressed = false;
                }
            }
            // 이전에 눌린 적 없음
            else
            {
                // 현재는 눌린 상태임
                if (GetAsyncKeyState(key_value[i]) & 0x8001)
                {
                    m_keyboardInfo[i].state = Key_state::TAP;
                    m_keyboardInfo[i].isPrevPressed = true;
                    m_Command.push_back((Keyboard)i);
                    m_CommandTimeout = 0.f;
                }
                // 계속 안눌려있었음
                else
                {
                    m_keyboardInfo[i].state = Key_state::NONE;
                }
            }
        }

        // 마우스 상태 업데이트
        POINT point;
        GetCursorPos(&point);
        ScreenToClient(CEngine::GetInst()->GetMainWndHwnd(), &point);

        // 커맨드 갱신
        m_CommandTimeout += DT;
        if (m_CommandTimeout > 0.3)
        {
            m_CommandTimeout = 0.f;
            m_Command.clear();
        }

        // 이전 프레임 마우스좌표 기록
        m_MousePrevPos = m_MousePos;

        // 현재 마우스 좌표 기록
        m_MousePos = Vec2((float)point.x, (float)point.y);

        // 마우스 커서가 진행하는 방향벡터를 구함
        m_DragDir = m_MousePos - m_MousePrevPos;

        // 커서 이미지가 있는 경우, 디폴트 커서를 숨기고 커서 이미지로 대체
        if (m_Cursor->MeshRender()->GetMaterial()->GetTexParam(TEX_0) == nullptr)
        {
            CURSOR_ON;
        }
        else
            CURSOR_OFF;

        Vec2 resolution = CEngine::GetInst()->GetResolution();
        Vec2 clip = Vec2((m_MousePos.x / resolution.x) * 2.f - 1.f, -((m_MousePos.y / resolution.y) * 2.f - 1.f));
        m_Cursor->MeshRender()->GetMaterial()->SetConstParam(VEC2_0, clip);

    }
    else
    {
        for (size_t i = 0; i < m_keyboardInfo.size(); ++i)
        {
            if (Key_state::TAP == m_keyboardInfo[i].state
                || Key_state::PRESSED == m_keyboardInfo[i].state)
            {
                m_keyboardInfo[i].state = Key_state::RELEASE;
            }

            else if (Key_state::RELEASE == m_keyboardInfo[i].state)
            {
                m_keyboardInfo[i].state = Key_state::NONE;
            }

            m_keyboardInfo[i].isPrevPressed = false;
        }

        CURSOR_ON;

        // 마우스 좌표 갱신
        *((int*)&m_MousePos.x) = 0xffffffff;
        *((int*)&m_MousePos.y) = 0xffffffff;
    }

}

void CKeyMgr::Render()
{
    if (m_Cursor->MeshRender()->GetMaterial()->GetTexParam(TEX_0) == nullptr)
        return;
    CRenderMgr::GetInst()->GetUICam()->Camera()->SetMatrix();
    m_Cursor->Render();
}

bool CKeyMgr::IsMouseOffScreen()
{
    Vec2 vResolution = CEngine::GetInst()->GetResolution();

    if (vResolution.x <= m_MousePos.x || vResolution.y <= m_MousePos.y
        || m_MousePos.x < 0 || m_MousePos.y < 0)
    {
        return true;
    }
    else
    {
        return false;
    }
}