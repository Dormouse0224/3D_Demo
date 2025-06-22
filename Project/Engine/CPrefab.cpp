#include "pch.h"
#include "CPrefab.h"

#include "CGameObject.h"
#include "CPathMgr.h"
#include "CAssetMgr.h"
#include "CLevelMgr.h"
#include "CTaskMgr.h"
#include "CLevel.h"

CPrefab::CPrefab()
	: CAsset(ASSET_TYPE::PREFAB)
{
}

CPrefab::~CPrefab()
{
	delete m_PrefabObject;
}

void CPrefab::ConvertToPrefab(CGameObject* _Obj)
{
	m_PrefabObject = _Obj->Clone();
	CAssetMgr::GetInst()->AddAsset(m_PrefabObject->GetName() + L"_" + to_wstring(m_PrefabObject->GetID()), this);
	Save(CPathMgr::GetFileName(GetName()));
}

CGameObject* CPrefab::Instantiate()
{
	return m_PrefabObject->Clone();
}

int CPrefab::Save(const wstring& _FileName, bool _Update)
{
	wstring SaveDir = CPathMgr::GetContentDir();
	if (_Update)
	{
		// 버전 업데이트 신규 파일인 경우 업데이트 디렉토리에 저장
		std::filesystem::path parentDir = SaveDir;
		SaveDir = parentDir.parent_path().parent_path();
		SaveDir += L"\\Update\\";
	}
	std::filesystem::path path = SaveDir + std::wstring(L"Prefab\\") + _FileName + std::wstring(L".prefab");
	CPathMgr::CreateParentDir(path);
	std::fstream file(path, std::ios::out | std::ios::binary);

	if (FAILED(m_PrefabObject->Save(file, _Update)))
		return E_FAIL;

	file.close();
	return S_OK;
}

int CPrefab::Load(const wstring& _FilePath)
{
	std::filesystem::path path = CPathMgr::GetContentDir() + _FilePath;
	std::fstream file(path, std::ios::in | std::ios::binary);

	m_PrefabObject = new CGameObject;
	if (FAILED(m_PrefabObject->Load(file)))
		return E_FAIL;

	file.close();
	return S_OK;
}
