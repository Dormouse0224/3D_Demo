#include "pch.h"
#include "CUICom.h"

#include "CKeyMgr.h"
#include "CEngine.h"
#include "CAssetMgr.h"

#include "CTransform.h"
#include "CFont.h"

CUICom::CUICom()
	:CRenderComponent(COMPONENT_TYPE::UICOM)
	, m_ClickEvent(nullptr)
	, m_KeydownEvent(nullptr)
	, m_MouseHover(false)
	, m_Clicked(false)
	, m_vecUIText{}
	, m_ScreenPosNDC()
	, m_Scale()
	, m_Alpha(1.f)
	, m_Active(false)
{
}

CUICom::CUICom(const CUICom& _Other)
	:CRenderComponent(_Other)
	, m_ClickEvent(_Other.m_ClickEvent)
	, m_KeydownEvent(_Other.m_KeydownEvent)
	, m_MouseHover(false)
	, m_Clicked(false)
	, m_vecUIText{}
	, m_ScreenPosNDC(_Other.m_ScreenPosNDC)
	, m_Scale(_Other.m_Scale)
	, m_Alpha(1.f)
	, m_Active(false)
{
	for (int i = 0; i < _Other.m_vecUIText.size(); ++i)
	{
		tUITextDesc* pDesc = new tUITextDesc;
		pDesc->Color = get<2>(_Other.m_vecUIText[i])->Color;
		pDesc->Rot = get<2>(_Other.m_vecUIText[i])->Rot;
		pDesc->Scale = get<2>(_Other.m_vecUIText[i])->Scale;
		m_vecUIText.push_back(make_tuple(get<0>(_Other.m_vecUIText[i]), get<1>(_Other.m_vecUIText[i]), pDesc));
	}
}

CUICom::~CUICom()
{
	for (auto& tuple : m_vecUIText)
		delete get<2>(tuple);
}

void CUICom::FinalTick()
{
	// point mesh 로 고정
	SetMesh(CAssetMgr::GetInst()->Load<CMesh>(L"EA_RectMesh", true));

	if (m_Active)
	{
		// UI 의 화면상 위치 계산 후 마우스 커서의 호버링 체크
		Vec2 cursor = CKeyMgr::GetInst()->GetMousePos();
		Vec2 resolution = CEngine::GetInst()->GetResolution();
		Vec2 ScreenPos(((m_ScreenPosNDC.x + 1) / 2.f) * resolution.x, ((1 - m_ScreenPosNDC.y) / 2.f) * resolution.y);

		Vec2 ScreenLT(ScreenPos - Vec2(m_Scale / 2.f));
		Vec2 ScreenRB(ScreenPos + Vec2(m_Scale / 2.f));

		if (ScreenLT.x < cursor.x && ScreenLT.y < cursor.y && ScreenRB.x > cursor.x && ScreenRB.y > cursor.y)
			m_MouseHover = true;
		else
			m_MouseHover = false;

		// 마우스 좌클릭 이벤트 체크
		if (CKeyMgr::GetInst()->GetKeyState(Keyboard::MOUSE_LBTN) == Key_state::TAP && m_MouseHover)
			m_Clicked = true;

		if (CKeyMgr::GetInst()->GetKeyState(Keyboard::MOUSE_LBTN) == Key_state::PRESSED && m_MouseHover && m_Clicked)
		{
			if (m_KeydownEvent)
				m_KeydownEvent(GetOwner());
		}

		if (CKeyMgr::GetInst()->GetKeyState(Keyboard::MOUSE_LBTN) == Key_state::RELEASE && m_MouseHover && m_Clicked)
		{
			m_Clicked = false;
			if (m_ClickEvent)
				m_ClickEvent(GetOwner());
		}

		if (CKeyMgr::GetInst()->GetKeyState(Keyboard::MOUSE_LBTN) == Key_state::NONE)
			m_Clicked = false;
	}
	else
	{
		m_MouseHover = false;
		m_Clicked = false;
	}


}

int CUICom::Save(fstream& _Stream)
{
	if (FAILED(CRenderComponent::RenderCom_Save(_Stream)))
		return E_FAIL;

	// 텍스트 정보 개수 저장
	int count = m_vecUIText.size();
	_Stream.write(reinterpret_cast<char*>(&count), sizeof(int));

	for (int i = 0; i < count; ++i)
	{
		// 폰트 에셋을 이름으로 저장
		std::wstring fontName = get<0>(m_vecUIText[i])->GetName();
		int size = fontName.length();
		_Stream.write(reinterpret_cast<char*>(&size), sizeof(int));
		if (size > 0)
		{
			_Stream.write(reinterpret_cast<char*>(fontName.data()), sizeof(wchar_t) * size);
		}

		// 텍스트 저장
		std::wstring text = get<1>(m_vecUIText[i]);
		size = text.length();
		_Stream.write(reinterpret_cast<char*>(&size), sizeof(int));
		if (size > 0)
		{
			_Stream.write(reinterpret_cast<char*>(text.data()), sizeof(wchar_t) * size);
		}

		// 텍스트 디스크립션 저장
		tUITextDesc* pTextDesc = get<2>(m_vecUIText[i]);
		_Stream.write(reinterpret_cast<char*>(pTextDesc), sizeof(tUITextDesc));
	}

	_Stream.write(reinterpret_cast<char*>(&m_ScreenPosNDC), sizeof(Vec2));
	_Stream.write(reinterpret_cast<char*>(&m_Scale), sizeof(Vec2));

	return S_OK;
}

int CUICom::Load(fstream& _Stream)
{
	if (FAILED(CRenderComponent::RenderCom_Load(_Stream)))
		return E_FAIL;

	// 텍스트 정보 개수 읽어오기
	int count = 0;
	_Stream.read(reinterpret_cast<char*>(&count), sizeof(int));

	for (int i = 0; i < count; ++i)
	{
		// 저장된 이름으로 폰트 에셋 로드
		int size = 0;
		std::wstring fontName = {};
		AssetPtr<CFont> pFont = nullptr;
		_Stream.read(reinterpret_cast<char*>(&size), sizeof(int));
		if (size > 0)
		{
			fontName.resize(size);
			_Stream.read(reinterpret_cast<char*>(fontName.data()), sizeof(wchar_t) * size);
			pFont = CAssetMgr::GetInst()->Load<CFont>(fontName);
		}

		// 저장된 텍스트 읽어오기
		std::wstring text = {};
		_Stream.read(reinterpret_cast<char*>(&size), sizeof(int));
		if (size > 0)
		{
			text.resize(size);
			_Stream.read(reinterpret_cast<char*>(text.data()), sizeof(wchar_t) * size);
		}

		// 저장된 텍스트 디스크립션 읽어오기
		tUITextDesc* pTextDesc = new tUITextDesc;
		_Stream.read(reinterpret_cast<char*>(pTextDesc), sizeof(tUITextDesc));

		// 폰트가 성공적으로 로딩된 경우에만 텍스트 벡터에 저장
		if (pFont.Get())
		{
			AddUIText(pFont, text, pTextDesc);
		}
	}

	// UI 의 스크린상 위치와 스케일 읽어오기
	_Stream.read(reinterpret_cast<char*>(&m_ScreenPosNDC), sizeof(Vec2));
	_Stream.read(reinterpret_cast<char*>(&m_Scale), sizeof(Vec2));

	return S_OK;
}

wstring CUICom::GetUIText(int _Idx)
{
	if (m_vecUIText.size() > _Idx)
		return get<1>(m_vecUIText[_Idx]);
	else
		return L"";
}

void CUICom::SetUIText(int _Idx, wstring _wstr)
{
	if (m_vecUIText.size() > _Idx)
		get<1>(m_vecUIText[_Idx]) = _wstr;
}

void CUICom::Render()
{
	// 위치정보 업데이트
	Transform()->Binding();

	// 사용할 쉐이더 바인딩
	GetMaterial()->SetScalarParam(VEC2_0, m_ScreenPosNDC);
	m_Alpha = clamp(m_Alpha, 0.f, 1.f);
	GetMaterial()->SetScalarParam(FLOAT_0, m_Alpha);
	GetMaterial()->SetScalarParam(FLOAT_1, m_Scale.x);
	GetMaterial()->SetScalarParam(FLOAT_2, m_Scale.y);
	GetMaterial()->Binding();

	// 렌더링 시작
	GetMesh()->Render();

	// 텍스트 렌더링 (렌더 컴포넌트 호출 후 별도로 호출됨)
	Vec2 resolution = CEngine::GetInst()->GetResolution();
	Vec2 ScreenPos(((m_ScreenPosNDC.x + 1) / 2.f) * resolution.x, ((1 - m_ScreenPosNDC.y) / 2.f) * resolution.y);
	for (auto& tuple : m_vecUIText)
	{
		Vec4 color = get<2>(tuple)->Color;
		color *= m_Alpha;
		get<0>(tuple)->Render(get<1>(tuple), ScreenPos, color, get<2>(tuple)->Rot, get<2>(tuple)->Scale, 0);
	}
}
