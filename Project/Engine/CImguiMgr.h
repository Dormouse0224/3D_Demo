#pragma once

#include "imgui/imgui.h"
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_impl_dx11.h"

enum PopupFlags_
{
	PopupFlags_AddGameObjectMenu	= (1<<0),
	PopupFlags_NewLevel				= (1<<1),
	PopupFlags_SaveLevel			= (1<<2),
	PopupFlags_CreateMaterial		= (1<<3),
	PopupFlags_CreateFlipbook		= (1<<4),
	PopupFlags_CreateSprite			= (1<<5),
};

class CImguiObject;

class CInspector;
class COutliner;
class CContentViewer;

class CImguiMgr
	: public Singleton<CImguiMgr>
{
	SINGLE(CImguiMgr);
private:
	map<string, CImguiObject*>	m_mapImguiObj;

	bool						m_DebugMenuBar;
	bool						m_DemoActive;

	ULONGLONG					m_PopupFlag;

	CInspector*					m_Inspector;
	COutliner*					m_Outliner;
	CContentViewer*				m_ContentViewer;

public:
	void Init();
	void Progress();
	void Render();
	void DebugMenuBar();

	void AddGameObjectMenuPopup();
	void NewLevelPopup();
	void SaveLevelPopup();
	void CreateMaterialPopup();
	void CreateFlipbookPopup();
	void CreateSpritePopup();

	void LoadAsset();
	void LoadLevel();

	CInspector* GetInspector() { return m_Inspector; }
	COutliner* GetOutliner() { return m_Outliner; }
};

