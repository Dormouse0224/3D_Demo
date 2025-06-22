#pragma once

class CPathMgr
{
private:
	static wchar_t g_ContentDir[255];
	static wchar_t g_BinPath[255];
	static wchar_t g_SolutionPath[255];

public:
	static void Init();
	static const wchar_t* GetContentDir() { return g_ContentDir; }
	static const wchar_t* GetBinPath() { return g_BinPath; }
	static const wchar_t* GetSolutionPath() { return g_SolutionPath; }
	static void CreateParentDir(std::filesystem::path _Path);
	static const wstring GetFileName(std::filesystem::path _Path);

private:
	CPathMgr() {}
	~CPathMgr() {}
};

