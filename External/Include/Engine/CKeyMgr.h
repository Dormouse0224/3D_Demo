#pragma once

// 키보드 키 종류
enum class Keyboard
{
    SC_graveAccent,
    SC_hyphen,
    SC_equal,
    SC_bracketLeft,
    SC_bracketRight,
    SC_backslash,
    SC_semicolon,
    SC_apostrophe,
    SC_comma,
    SC_period,
    SC_slash,

    Q,
    W,
    E,
    R,
    T,
    Y,
    U,
    I,
    O,
    P,
    A,
    S,
    D,
    F,
    G,
    H,
    J,
    K,
    L,
    Z,
    X,
    C,
    V,
    B,
    N,
    M,

    NUM_1,
    NUM_2,
    NUM_3,
    NUM_4,
    NUM_5,
    NUM_6,
    NUM_7,
    NUM_8,
    NUM_9,
    NUM_0,

    F1,
    F2,
    F3,
    F4,
    F5,
    F6,
    F7,
    F8,
    F9,
    F10,
    F11,
    F12,

    ESC,
    TAB,
    CAPSLOCK,
    LSHIFT,
    LCTRL,
    LALT,
    BACKSPACE,
    ENTER,
    RSHIFT,
    RALT,
    RCTRL,
    SPACE,

    UP,
    DOWN,
    LEFT,
    RIGHT,

    NUMPAD_period,
    NUMPAD_0,
    NUMPAD_1,
    NUMPAD_2,
    NUMPAD_3,
    NUMPAD_4,
    NUMPAD_5,
    NUMPAD_6,
    NUMPAD_7,
    NUMPAD_8,
    NUMPAD_9,

    MOUSE_LBTN,
    MOUSE_RBTN,

    Keyboard_end
};

// 키보드 키 상태
enum class Key_state
{
    NONE,
    TAP,
    RELEASE,
    PRESSED,
};


struct key_info
{
    Key_state state;
    bool isPrevPressed;
};

class CTexture2D;

class CKeyMgr
    : public Singleton<CKeyMgr>
{
public:
    SINGLE(CKeyMgr);

private:
    vector<key_info>    m_keyboardInfo;
    Vec2                m_MousePos;
    Vec2                m_MousePrevPos;
    Vec2                m_DragDir;

    vector<Keyboard>    m_Command;
    float               m_CommandTimeout;

    CGameObject*        m_Cursor;

public:
    Key_state GetKeyState(const Keyboard& _key) { return m_keyboardInfo[(int)_key].state; }
    Vec2 GetMousePos() { return m_MousePos; }
    Vec2 GetMousePrevPos() { return m_MousePos; }
    Vec2 GetDragDir() { return m_DragDir; }

    void SetCursorTex(AssetPtr<CTexture2D> _Tex);

    void Init();
    void Tick();
    void Render();

    bool IsMouseOffScreen();

    const vector<Keyboard>& GetCommand() { return m_Command; }
    void ClearCommand() { m_Command.clear(); }
};

