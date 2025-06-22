#include "pch.h"
#include "CPathMgr.h"

vector<wstring> g_vecScriptNames;
vector<wstring> g_vecStateNames;
vector<wstring> g_vecTFNames;

int main()
{
	CPathMgr::Init();
	wstring strProjPath = CPathMgr::GetProjectPath();
    wstring strHeaderPath = strProjPath + L"Script\\CScriptInit.h";
	wstring strCppPath = strProjPath + L"Script\\CScriptInit.cpp";
    wstring strTFCode = strProjPath + L"Script\\TriggerFunc.h";

	// Script, State, TriggerFunc 파일 위치
	wstring strScriptIncludePath = CPathMgr::GetIncludePath();
	wstring strScriptCode = strScriptIncludePath + L"Script\\Scripts\\";
    CPathMgr::CreateDir(strScriptCode);
	wstring strStateCode = strScriptIncludePath + L"Script\\States\\";
    CPathMgr::CreateDir(strStateCode);

	// 예외 리스트 목록을 알아낸다.
	std::wfstream Exception(L"Exeptionlist.txt", ios::in);

	vector<wstring> vecExcept;

	if (Exception.is_open())
	{
		wstring Name = L"";
		while (getline(Exception, Name))
		{
			vecExcept.push_back(Name);
		}
		Exception.close();
	}

	// Scriipt 목록 벡터 작성
	WIN32_FIND_DATA tData = {};
	HANDLE handle = FindFirstFile(wstring(strScriptCode + L"\\*.h").c_str(), &tData);
	if (INVALID_HANDLE_VALUE != handle)
	{

		while (true)
		{
			// 예외가 아닌경우, 스크립트 이름으로 본다.
			bool bExeption = false;
			for (size_t i = 0; i < vecExcept.size(); ++i)
			{
				if (!wcscmp(tData.cFileName, vecExcept[i].c_str()))
				{
					bExeption = true;
					break;
				}
			}

			if (!bExeption)
			{
				// -2 는 .h 확장자를 지우기 위함.
				g_vecScriptNames.push_back(wstring(tData.cFileName).substr(0, wcslen(tData.cFileName) - 2));
			}

			if (!FindNextFile(handle, &tData))
				break;
		}
		FindClose(handle);
	}

	// State 목록 벡터 작성
	tData = {};
	handle = FindFirstFile(wstring(strStateCode + L"\\*.h").c_str(), &tData);
	if (INVALID_HANDLE_VALUE != handle)
	{
		while (true)
		{
			// 예외가 아닌경우, 스크립트 이름으로 본다.
			bool bExeption = false;
			for (size_t i = 0; i < vecExcept.size(); ++i)
			{
				if (!wcscmp(tData.cFileName, vecExcept[i].c_str()))
				{
					bExeption = true;
					break;
				}
			}

			if (!bExeption)
			{
				// -2 는 .h 확장자를 지우기 위함.
				g_vecStateNames.push_back(wstring(tData.cFileName).substr(0, wcslen(tData.cFileName) - 2));
			}

			if (!FindNextFile(handle, &tData))
				break;
		}
		FindClose(handle);
	}

	// Trigger Function 목록 작성
	fstream TFfile(strTFCode, std::ios::in);
	if (TFfile.is_open())
	{
		string line;
		while (getline(TFfile, line))
		{
			int pos0 = 0;
			if ((pos0 = line.find("bool ")) != string::npos)
			{
				pos0 += 5;
				while (line[pos0] == ' ')
					++pos0;
				// pos0 는 함수 이름 시작 인덱스
				int pos1 = 0;
				if ((pos1 = line.find('(', pos0)) != string::npos)
				{
					while (line[pos1 - 1] == ' ')
						--pos1;
					// pos1 는 함수 이름 끝 인덱스
					string FuncName = line.substr(pos0, pos1 - pos0);
					int pos2 = 0;
					if ((pos2 = line.find("CFSM_State*", pos1)) != string::npos)
					{
						if ((pos2 = line.find("CFSM_State*", pos2 + 11)) != string::npos)
						{
							if (line.find("CFSM_State*", pos2 + 11) == string::npos && line.find(';', pos2 + 11) != string::npos)
							{
								g_vecTFNames.push_back(wstring(FuncName.begin(), FuncName.end()));
							}
						}
					}
				}
			}
		}
        TFfile.close();
	}
    else
    {
        // 파일이 없는 경우 만듦
        wfstream TFfile(strTFCode, std::ios::out);
        if (TFfile.is_open())
        {
            TFfile.imbue(std::locale("kor"));

            TFfile << L"#pragma once" << std::endl;
            TFfile << std::endl;
            TFfile.close();
        }
    }

    //========================
    // CScriptInit header 작성
    //========================

    wfstream headerfile(strHeaderPath, ios::out);
    if (!headerfile.is_open())
        return 0;
    headerfile.imbue(std::locale("kor"));

    // 매크로 입력
    headerfile << L"#pragma once" << std::endl;
    headerfile << std::endl;

    headerfile << L"#define REGISTER_SCRIPT(Type) CComponentMgr::GetInst()->RegisterScript(typeid(Type).name()\\" << std::endl;
    headerfile << L", []() -> CScript* { CScript* pScript = new Type; pScript->SetName(to_wstr(typeid(Type).name())); return pScript; });" << std::endl;
    headerfile << L"#define REGISTER_STATE(Type) CFSMMgr::GetInst()->RegisterState(typeid(Type).name()\\" << std::endl;
    headerfile << L", []() -> CFSM_State* { CFSM_State* pState = new Type; pState->SetName(to_wstr(typeid(Type).name())); return pState; });" << std::endl;
    headerfile << L"#define REGISTER_TRIGGER(Func) CFSMMgr::GetInst()->RegisterTrigger(#Func, &Func);" << std::endl;
    headerfile << std::endl;

    headerfile << L"// ========================================================" << std::endl;
    headerfile << L"// 이 코드는 CodeGen 코드 생성기에 의해 작성되었습니다." << std::endl;
    headerfile << L"// Exeptionlist.txt 는 해당 파일 내 이름들을 예외처리합니다." << std::endl;
    headerfile << L"// ScriptList.txt 는 코드가 생성된 스크립트와 상태 클래스 목록입니다." << std::endl;
    headerfile << L"// FuncList.txt 는 코드가 생성된 상태 변경 함수 목록입니다." << std::endl;
    headerfile << L"// 코드는 항상 갱신됩니다." << std::endl;
    headerfile << L"// ========================================================" << std::endl;
    headerfile << std::endl;

    headerfile << L"class CScriptInit" << std::endl;
    headerfile << L"{" << std::endl;
    headerfile << L"public:" << std::endl;
    headerfile << L"\tstatic void ScriptInit();" << std::endl;
    headerfile << L"\tstatic void FSMInit();" << std::endl;
    headerfile << L"};" << std::endl;
    headerfile << std::endl;

    headerfile.close();


	//=====================
	// CScriptInit cpp 작성
	//=====================

	wfstream cppfile(strCppPath, ios::out);
	if (!cppfile.is_open())
		return 0;
    cppfile.imbue(std::locale("kor"));

	// 헤더 입력
	cppfile << L"#include \"pch.h\"" << std::endl;
	cppfile << L"#include \"CScriptInit.h\"" << std::endl;
	cppfile << L"#include \"TriggerFunc.h\"" << std::endl;
	cppfile << L"#include \"Engine\\CComponentMgr.h\"" << std::endl;
	cppfile << L"#include \"Engine\\CFSMMgr.h\"" << std::endl;
	cppfile << std::endl;

	cppfile << L"// ========================================================" << std::endl;
	cppfile << L"// 이 코드는 CodeGen 코드 생성기에 의해 작성되었습니다." << std::endl;
	cppfile << L"// Exeptionlist.txt 는 해당 파일 내 이름들을 예외처리합니다." << std::endl;
    cppfile << L"// ScriptList.txt 는 코드가 생성된 스크립트와 상태 클래스 목록입니다." << std::endl;
    cppfile << L"// FuncList.txt 는 코드가 생성된 상태 변경 함수 목록입니다." << std::endl;
	cppfile << L"// 코드는 항상 갱신됩니다." << std::endl;
	cppfile << L"// ========================================================" << std::endl;
	cppfile << std::endl;

	// Script 헤더
	for (UINT i = 0; i < g_vecScriptNames.size(); ++i)
	{
        cppfile << L"#include \"Scripts\\" << g_vecScriptNames[i] << L".h\"" << std::endl;
	}
    cppfile << std::endl;
	//State 헤더
	for (UINT i = 0; i < g_vecStateNames.size(); ++i)
	{
        cppfile << L"#include \"States\\" << g_vecStateNames[i] << L".h\"" << std::endl;
	}
    cppfile << std::endl;

	// Script 등록 함수 작성
    cppfile << L"void CScriptInit::ScriptInit()" << std::endl;
    cppfile << L"{" << std::endl;
	for (UINT i = 0; i < g_vecScriptNames.size(); ++i)
	{
        cppfile << L"\tREGISTER_SCRIPT(" << g_vecScriptNames[i] << L");" << std::endl;
	}
    cppfile << L"}" << std::endl;
    cppfile << std::endl;

	// State, TriggerFunc 등록 함수 작성
    cppfile << L"void CScriptInit::FSMInit()" << std::endl;
    cppfile << L"{" << std::endl;
	for (UINT i = 0; i < g_vecStateNames.size(); ++i)
	{
        cppfile << L"\tREGISTER_STATE(" << g_vecStateNames[i] << L");" << std::endl;
	}
    cppfile << std::endl;
	for (UINT i = 0; i < g_vecTFNames.size(); ++i)
	{
        cppfile << L"\tREGISTER_TRIGGER(" << g_vecTFNames[i] << L");" << std::endl;
	}
    cppfile << L"}" << std::endl;
    cppfile << std::endl;

    cppfile.close();


	// 다음번 실행시 비교하기위한 정보 저장
	wfstream ScriptList(L"ScriptList.txt", ios::out);
	if (!ScriptList.is_open())
		return 0;

	for (UINT i = 0; i < g_vecScriptNames.size(); ++i)
	{
		ScriptList << g_vecScriptNames[i] << std::endl;
	}
	for (UINT i = 0; i < g_vecStateNames.size(); ++i)
	{
		ScriptList << g_vecStateNames[i] << std::endl;
	}
	ScriptList.close();


	wfstream FuncList(L"FuncList.txt", ios::out);
	if (!FuncList.is_open())
		return 0;

	for (UINT i = 0; i < g_vecTFNames.size(); ++i)
	{
		FuncList << g_vecTFNames[i] << std::endl;
	}
	FuncList.close();

	return 0;
}