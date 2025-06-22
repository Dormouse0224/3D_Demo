#include "pch.h"
#include "CLevel.h"

#include "CLayer.h"
#include "CGameObject.h"

#include "CTaskMgr.h"
#include "CLevelMgr.h"
#include "CPathMgr.h"

CLevel::CLevel()
	: m_Layer{}
	, m_State(LEVEL_STATE::STOP)
{
	for (UINT i = 0; i < (UINT)LAYER::END; ++i)
	{
		m_Layer[i] = new CLayer;
		m_Layer[i]->SetLayerIdx(i);
	}
}

CLevel::CLevel(const CLevel& _Other)
	: CEntity(_Other)
	, m_Layer{}
	, m_State(LEVEL_STATE::STOP)
{
	for (UINT i = 0; i < (UINT)LAYER::END; ++i)
	{
		m_Layer[i] = _Other.m_Layer[i]->Clone();
	}
}

CLevel::~CLevel()
{
	for (UINT i = 0; i < (UINT)LAYER::END; ++i)
	{
		delete m_Layer[i];
	}
}

void CLevel::Begin()
{
	for (UINT i = 0; i < (UINT)LAYER::END; ++i)
	{
		m_Layer[i]->Begin();
	}
}

void CLevel::End()
{
	for (UINT i = 0; i < (UINT)LAYER::END; ++i)
	{
		m_Layer[i]->End();
	}
}

void CLevel::Tick()
{
	for (UINT i = 0; i < (UINT)LAYER::END; ++i)
	{
		m_Layer[i]->Tick();
	}
}

void CLevel::FinalTick()
{
	for (UINT i = 0; i < (UINT)LAYER::END; ++i)
	{
		m_Layer[i]->FinalTick();
	}
}

void CLevel::SetState(LEVEL_STATE _State)
{
	// 현재 상태와, 변경할 상태가 동일한 경우 무시
	if (m_State == _State)
		return;
	
	// Stop -> Pause 는 불가능하므로 무시
	if (m_State == LEVEL_STATE::STOP && _State == LEVEL_STATE::PAUSE)
		return;

	// Stop -> Play
	if (m_State == LEVEL_STATE::STOP && _State == LEVEL_STATE::PLAY)
	{
		m_State = _State;
		CLevelMgr::GetInst()->SetPlayStartLevel(CLevelMgr::GetInst()->GetCurrentLevel()->Clone());
		Begin();
	}
	// Play -> Stop
	// Pause -> Stop
	else if (_State == LEVEL_STATE::STOP)
	{
		m_State = _State;
		End();
		CTaskMgr::GetInst()->AddTask(TASK_TYPE::CHANGE_LEVEL, reinterpret_cast<DWORD_PTR>(CLevelMgr::GetInst()->GetPlayStartLevel()), (DWORD_PTR)false);
	}
	else
	// Play -> Pause
	// Pause -> Play
	{
		m_State = _State;
	}

}

void CLevel::Deregister()
{
	for (UINT i = 0; i < (UINT)LAYER::END; ++i)
	{
		m_Layer[i]->ClearVecObjects();
	}
}

void CLevel::Register()
{
	for (UINT i = 0; i < (UINT)LAYER::END; ++i)
	{
		m_Layer[i]->Register();
	}
}

int CLevel::Save(wstring _FileName, bool _Update)
{
	wstring RelativePath = std::wstring(L"Level\\") + _FileName + std::wstring(L".level");
	wstring SaveDir = CPathMgr::GetContentDir();
	if (_Update)
	{
		// 버전 업데이트 신규 파일인 경우 업데이트 디렉토리에 저장
		std::filesystem::path parentDir = SaveDir;
		SaveDir = parentDir.parent_path().parent_path();
		SaveDir += L"\\Update\\";
	}
	std::filesystem::path path = SaveDir + RelativePath;
	std::filesystem::path backup = SaveDir + RelativePath;
	CPathMgr::CreateParentDir(path);

	// 저장 실패를 대비해 백업
	backup.replace_filename(_FileName + L"_backup.level");
	if (filesystem::exists(path)) {
		filesystem::copy_file(path, backup, filesystem::copy_options::overwrite_existing);
	}

	std::fstream file(path, std::ios::out | std::ios::binary);
	if (!file.is_open())
		return E_FAIL;

	for (int i = 0; i < static_cast<int>(LAYER::END); ++i)
	{
		if (FAILED(m_Layer[i]->Save(file, _Update)))
			return E_FAIL;
	}

	file.close();

	// 기존에 등록된 레벨 중 같은 이름의 레벨이 있다면 업데이트
	CLevel* pLevel = CLevelMgr::GetInst()->FindLevel(RelativePath);
	if (pLevel)
	{
		CLevelMgr::GetInst()->DeleteLevel(RelativePath);
		CLevelMgr::GetInst()->AddLevelList(RelativePath, this);
	}

	return S_OK;
}

int CLevel::Load(wstring _FilePath)
{
	std::filesystem::path path = CPathMgr::GetContentDir() + _FilePath;
	std::fstream file(path, std::ios::in | std::ios::binary);
	if (!file.is_open())
		return E_FAIL;

	wstring Name = path.filename().wstring();
	Name = Name.substr(0, Name.find(path.extension()));
	SetName(Name);

	for (int i = 0; i < static_cast<int>(LAYER::END); ++i)
	{
		if (FAILED(m_Layer[i]->Load(file)))
			return E_FAIL;
	}

	file.close();

	// LevelMgr LevelList 에 추가
	//CLevelMgr::GetInst()->AddLevelList(_FilePath, this);

	return S_OK;
}

void CLevel::AddGameObject(CGameObject* _Object, int _LayerIdx, bool _bChildMove)
{
	m_Layer[_LayerIdx]->AddGameObject(_Object, _bChildMove);
}

void CLevel::UnregisterGameObject(CGameObject* _Object, int _LayerIdx)
{
	m_Layer[_LayerIdx]->UnRegisterParentObject(_Object);
}
