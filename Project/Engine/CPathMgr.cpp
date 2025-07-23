#include "pch.h"
#include "CPathMgr.h"

wstring CPathMgr::g_ContentDir = L"";
wstring CPathMgr::g_BinPath = L"";
wstring CPathMgr::g_SolutionPath = L"";

void CPathMgr::Init()
{
	wchar_t szBuff[255] = {};

	// ���� ���μ����� ���(����� ���� ������ ��� ����ó�� �ʿ�, ������Ʈ ����->Debug->�۾����͸� ����
    GetModuleFileName(NULL, szBuff, 255);
    wstring exeDir = szBuff;

    g_BinPath = exeDir.substr(0, exeDir.rfind(L'\\') + 1);

    g_SolutionPath = exeDir.substr(0, exeDir.rfind(L"\\WutheringWaves\\") + 16);
    g_ContentDir = g_SolutionPath + L"Output\\Content\\";
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

wstring CPathMgr::GetRelativePath(const wstring& _FilePath)
{
    wstring contentPath = GetContentDir();

    if (-1 == _FilePath.find(contentPath.c_str()))
    {
        return wstring();
    }

    return _FilePath.substr(contentPath.length(), _FilePath.length());
}

