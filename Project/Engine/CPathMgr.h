#pragma once

class CPathMgr
{
private:
	static wstring g_ContentDir;
	static wstring g_BinPath;
	static wstring g_SolutionPath;

public:
	static void Init();
	static const wstring GetContentDir() { return g_ContentDir; }
	static const wstring GetBinPath() { return g_BinPath; }
	static const wstring GetSolutionPath() { return g_SolutionPath; }
	static void CreateParentDir(std::filesystem::path _Path);
	static const wstring GetFileName(std::filesystem::path _Path);
    static wstring GetRelativePath(const wstring& _FilePath);

private:
	CPathMgr() {}
	~CPathMgr() {}
};

