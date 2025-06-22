#include "pch.h"
#include "CFont.h"

#include "CPathMgr.h"
#include "CRenderMgr.h"

#include "CDevice.h"

CFont::CFont()
	:CAsset(ASSET_TYPE::FONT)
{
	m_SpriteBatch = std::make_unique<SpriteBatch>(CONTEXT);
	m_SpriteFont = nullptr;
}

CFont::~CFont()
{
	//delete m_SpriteBatch;
	//delete m_SpriteFont;
}

int CFont::Load(const wstring& _FilePath)
{
	std::wstring path = CPathMgr::GetContentDir() + _FilePath;
	//if (m_SpriteFont)
		//delete m_SpriteFont;
	m_SpriteFont = std::make_unique<SpriteFont>(DEVICE, path.c_str());

	return S_OK;
}

void CFont::Render(wstring _Text, Vec2 _Pos, Vec4 _Color, float _Rot, float _Scale, float _Depth)
{
	// 블렌드 설정
	CRenderMgr::GetInst()->UnbindShaders();
	m_SpriteBatch->Begin();
	if (m_SpriteFont)
	{
		Vec2 origin = m_SpriteFont->MeasureString(_Text.c_str()) / 2.f;
		//m_SpriteFont->DrawString(m_SpriteBatch.get(), L"TEST", DirectX::XMFLOAT2(0, 0), DirectX::Colors::White);
		m_SpriteFont->DrawString(m_SpriteBatch.get(), _Text.c_str(), _Pos, _Color, _Rot, origin, _Scale, SpriteEffects_None, _Depth);
	}
	m_SpriteBatch->End();	// 드로우콜
}
