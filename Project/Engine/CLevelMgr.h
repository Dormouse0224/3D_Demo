#pragma once

#include "CLevel.h"
class CGameObject;

class CLevelMgr
	: public Singleton<CLevelMgr>
{
	SINGLE(CLevelMgr);
private:
	CLevel*					m_CurLevel;
	CLevel*					m_PlayStartLevel;
	map<wstring, CLevel*>	m_mapLevelList;
	bool					m_LevelModified;
	bool					m_LevelModifyChecker;



public:
	void Init();
	void Progress();

	CLevel* GetCurrentLevel() { return m_CurLevel; }
	bool GetLevelModified() { return m_LevelModified; }
	const CLevel* GetPlayStartLevel() { return m_PlayStartLevel; }

	void SetCurrentLevel(CLevel* _Level) { m_CurLevel = _Level; }
	void SetLevelModified() { m_LevelModifyChecker = true; }
	void SetPlayStartLevel(CLevel* _Level);

	// 입력받은 레벨을 복사하여 상대경로를 키값으로 저장합니다.
	void AddLevelList(wstring _RelativePath, CLevel* _Level) { m_mapLevelList.insert(make_pair(_RelativePath, _Level->Clone())); }
	// 상대경로를 키값으로 저장된 레벨을 찾아 복사본을 반환합니다.
	CLevel* FindLevel(wstring _RelativePath);
	// 저장된 레벨 중 해당 키값으로 등록된 레벨을 제거합니다.
	void DeleteLevel(wstring _RelativePath);

	// 현재 레벨에 신규 게임 오브젝트 객체를 추가합니다.
	CGameObject* AddGameObject(wstring _Name, LAYER _Layer);
	void ChangeLevel(CLevel* _NextLevel, bool _Play = false);

	CGameObject* FindObject(LAYER _Layer, wstring _Name);
	void DeleteObject(LAYER _Layer, wstring _Name);

};

