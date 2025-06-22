#include "pch.h"
#include "CPathMgr.h"

wchar_t CPathMgr::g_ContentDir[255] = {};
wchar_t CPathMgr::g_BinPath[255] = {};
wchar_t CPathMgr::g_SolutionPath[255] = {};

void CPathMgr::Init()
{
	wchar_t szBuff[255] = {};

	// 실행 프로세스의 경로(디버깅 모드로 실행할 경우 예외처리 필요, 프로젝트 구성->Debug->작업디렉터리 세팅
	GetCurrentDirectory(255, szBuff);

	wcscpy_s(g_BinPath, 255, szBuff);
#ifndef _DEBUG
	wcscat_s(g_BinPath, 255, L"\\bin\\");
#else
	wcscat_s(g_BinPath, 255, L"\\bin\\debug\\");
#endif

	wcscpy_s(g_ContentDir, 255, szBuff);
	wcscat_s(g_ContentDir, 255, L"\\Content\\");

	for (int i = wcslen(szBuff); 0 <= i; --i)
	{
		if (szBuff[i] == L'\\')
		{
			szBuff[i + 1] = L'\0';
			break;
		}
	}
	wcscpy_s(g_SolutionPath, 255, szBuff);
}

void CPathMgr::CreateParentDir(std::filesystem::path _Path)
{
	if (!std::filesystem::exists(_Path.parent_path()))
		std::filesystem::create_directories(_Path.parent_path());
}

const wstring CPathMgr::GetFileName(std::filesystem::path _Path)
{
	wstring wstr = _Path.filename().wstring().substr(0, _Path.filename().wstring().find(L'.'));
	return wstr;
}
