#pragma once
#include "CRenderComponent.h"

class CFont;

// 이벤트 함수
typedef void (*UIEventFunc)(CGameObject* _Owner);

struct tUITextDesc
{
    Vec4    Color;
    float   Rot;
    float   Scale;
};

class CUICom :
    public CRenderComponent
{
    friend class CUIComUI;
public:
    CUICom();
    CUICom(const CUICom& _Other);
    ~CUICom();
    CLONE(CUICom)

private:
    UIEventFunc     m_ClickEvent;
    UIEventFunc     m_KeydownEvent;
    bool            m_MouseHover;
    bool            m_Clicked;

    vector<tuple<AssetPtr<CFont>, wstring, tUITextDesc*>> m_vecUIText;

    Vec2            m_ScreenPosNDC;
    Vec2            m_Scale;
    float           m_Alpha;

    bool            m_Active;

public:
    virtual void FinalTick() override;

    virtual int Save(fstream& _Stream);
    virtual int Load(fstream& _Stream);

    bool GetMouseHover() { return m_MouseHover; }
    bool GetClicked() { return m_Clicked; }
    float GetAlpha() { return m_Alpha; }
    bool GetActive() { return m_Active; }
    wstring GetUIText(int _Idx);

    void SetClickEvent(UIEventFunc _Func) { m_ClickEvent = _Func; }
    void SetKeydownEvent(UIEventFunc _Func) { m_KeydownEvent = _Func; }
    void SetAlpha(float _Alpha) { m_Alpha = _Alpha; }
    void SetActive(bool _Active) { m_Active = _Active; }
    void SetUIText(int _Idx, wstring _wstr);

    void AddUIText(AssetPtr<CFont> _Font, wstring _Text, tUITextDesc* _Option) { m_vecUIText.push_back(make_tuple(_Font, _Text, _Option)); }
    void Render();
};

