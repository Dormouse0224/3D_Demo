#include "pch.h"
#include "CImguiMgr.h"

#include "CImguiObject.h".

#include "CLevelMgr.h"
#include "CPathMgr.h"
#include "CAssetMgr.h"
#include "CTaskMgr.h"
#include "CDevice.h"

#include "CEngine.h"
#include "CInspector.h"
#include "COutliner.h"
#include "CContentViewer.h"
#include "CLevel.h"
#include "CGameObject.h"
#include "CTransform.h"

CImguiMgr::CImguiMgr()
    : m_DebugMenuBar(true)
    , m_DemoActive(false)
    , m_PopupFlag(0)
{
    int a = 0;
}

CImguiMgr::~CImguiMgr()
{
    SetCurrentDirectory(CPathMgr::GetBinPath());

    string inipath = to_str(CPathMgr::GetBinPath()) + "imgui.ini";
    // Cleanup
    if (ImGui::GetCurrentContext() != nullptr)
    {
        ImGui::SaveIniSettingsToDisk(inipath.c_str());
        ImGui_ImplDX11_Shutdown();
        ImGui_ImplWin32_Shutdown();
        ImGui::DestroyContext();
    }

    for (auto& ui : m_mapImguiObj)
    {
        delete ui.second;
    }
    m_mapImguiObj.clear();
}



void CImguiMgr::Progress()
{
    // Start the Dear ImGui frame
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    // 메인 윈도우 메뉴바
    if (m_DebugMenuBar)
    {
        DebugMenuBar();
    }

    // Imgui Demo
    if (m_DemoActive)
    {
        ImGui::ShowDemoWindow(&m_DemoActive);
    }

    // 등록된 Imgui 객체 업데이트 및 렌더 준비
    for (const auto& ui : m_mapImguiObj)
    {
        if (!ui.second->GetParent())
            ui.second->Update_Progress();
    }

    for (const auto& ui : m_mapImguiObj)
    {
        if (!ui.second->GetParent())
            ui.second->Render_Progress();
    }


}

void CImguiMgr::Render()
{
    // Rendering
    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

    // Update and Render additional Platform Windows
    if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
    }
}

void CImguiMgr::AddGameObjectMenuPopup()
{
    if (m_PopupFlag & PopupFlags_AddGameObjectMenu)
    {
        m_PopupFlag ^= PopupFlags_AddGameObjectMenu;
        ImGui::OpenPopup("AddGameObjectMenu");
    }

    if (ImGui::BeginPopupModal("AddGameObjectMenu", NULL, ImGuiWindowFlags_AlwaysAutoResize))
    {
        float tab = 130.f;
        ImGui::Text("Adding GameObject instance to current level.");
        ImGui::NewLine();

        // 오브젝트 이름 입력
        ImGui::Text("Name: ");
        ImGui::SameLine(tab);
        static char Name[255] = {};
        ImGui::InputText("##Name", Name, 255);

        // 레이어 입력
        ImGui::Text("Layer select");
        static int layeridx = 0;
        string layerstr[static_cast<int>(LAYER::END)] = {};
        const char* layer[static_cast<int>(LAYER::END)] = {};
        for (int i = 0; i < static_cast<int>(LAYER::END); ++i)
        {
            layerstr[i] = to_str(LAYER_WSTR[i]);
            layer[i] = layerstr[i].c_str();
        }
        ImGui::Combo("##Layer", &layeridx, layer, static_cast<int>(LAYER::END));

        if (ImGui::Button("Add"))
        {
            if (CLevelMgr::GetInst()->GetCurrentLevel())
                CLevelMgr::GetInst()->AddGameObject(to_wstr(string(Name)), static_cast<LAYER>(layeridx))->AddComponent(new CTransform);
            layeridx = 0;
            memset(Name, 0, sizeof(Name));
            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();
        if (ImGui::Button("Cancel"))
        {
            layeridx = 0;
            memset(Name, 0, sizeof(Name));
            ImGui::CloseCurrentPopup();
        }


        ImGui::EndPopup();
    }
    
}

void CImguiMgr::SaveLevelPopup()
{
    if (m_PopupFlag & PopupFlags_SaveLevel)
    {
        m_PopupFlag ^= PopupFlags_SaveLevel;
        ImGui::OpenPopup("SaveLevel");
    }

    if (ImGui::BeginPopupModal("SaveLevel", NULL, ImGuiWindowFlags_AlwaysAutoResize))
    {
        float tab = 130.f;
        ImGui::Text("Save current level as file.");
        ImGui::NewLine();

        // 레벨 파일 이름 입력
        ImGui::Text("Name: ");
        ImGui::SameLine(tab);
        static char Name[255] = {};
        ImGui::InputText("##Name", Name, 255);
        std::wstring WName = to_wstr(std::string(Name));

        if (ImGui::Button("Save"))
        {
            CLevelMgr::GetInst()->GetCurrentLevel()->Save(WName);
            memset(Name, 0, sizeof(Name));
            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();
        if (ImGui::Button("Cancel"))
        {
            memset(Name, 0, sizeof(Name));
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }

}

void CImguiMgr::NewLevelPopup()
{
    if (m_PopupFlag & PopupFlags_NewLevel)
    {
        m_PopupFlag ^= PopupFlags_NewLevel;
        ImGui::OpenPopup("CreateLevel");
    }

    if (ImGui::BeginPopupModal("CreateLevel", NULL, ImGuiWindowFlags_AlwaysAutoResize))
    {
        float tab = 130.f;
        ImGui::Text("Create new level.");
        ImGui::NewLine();

        // 오브젝트 이름 입력
        ImGui::Text("Name: ");
        ImGui::SameLine(tab);
        static char Name[255] = {};
        ImGui::InputText("##Name", Name, 255);
        std::wstring WName = to_wstr(std::string(Name));

        if (ImGui::Button("Create"))
        {
            CLevel* Level = CLevelMgr::GetInst()->GetCurrentLevel();
            if (Level) { Level->Save(Level->GetName()); }
            CLevel* NewLevel = new CLevel;
            NewLevel->SetName(WName);
            CTaskMgr::GetInst()->AddTask(TASK_TYPE::CHANGE_LEVEL, reinterpret_cast<DWORD_PTR>(NewLevel), (DWORD_PTR)false);
            memset(Name, 0, sizeof(Name));
            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();
        if (ImGui::Button("Cancel"))
        {
            memset(Name, 0, sizeof(Name));
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }
}

void CImguiMgr::CreateMaterialPopup()
{
    if (m_PopupFlag & PopupFlags_CreateMaterial)
    {
        m_PopupFlag ^= PopupFlags_CreateMaterial;
        ImGui::OpenPopup("CreateMaterial");
    }

    if (ImGui::BeginPopupModal("CreateMaterial", NULL, ImGuiWindowFlags_AlwaysAutoResize))
    {
        float tab = 130.f;
        ImGui::Text("Create new material.");
        ImGui::NewLine();

        // 오브젝트 이름 입력
        ImGui::Text("Name: ");
        ImGui::SameLine(tab);
        static char Name[255] = {};
        ImGui::InputText("##Name", Name, 255);
        std::wstring WName = to_wstr(std::string(Name));

        if (ImGui::Button("Create"))
        {
            CMaterial::Create(WName);
            memset(Name, 0, sizeof(Name));
            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();
        if (ImGui::Button("Cancel"))
        {
            memset(Name, 0, sizeof(Name));
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }

}

void CImguiMgr::CreateFlipbookPopup()
{
    if (m_PopupFlag & PopupFlags_CreateFlipbook)
    {
        m_PopupFlag ^= PopupFlags_CreateFlipbook;
        ImGui::OpenPopup("CreateFlipbook");
    }

    if (ImGui::BeginPopupModal("CreateFlipbook", NULL, ImGuiWindowFlags_AlwaysAutoResize))
    {
        float tab = 130.f;
        ImGui::Text("Create new flipbook.");
        ImGui::NewLine();

        // 오브젝트 이름 입력
        ImGui::Text("Name: ");
        ImGui::SameLine(tab);
        static char Name[255] = {};
        ImGui::InputText("##Name", Name, 255);
        std::wstring WName = to_wstr(std::string(Name));

        if (ImGui::Button("Create"))
        {
            CFlipbook::Create(WName);
            memset(Name, 0, sizeof(Name));
            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();
        if (ImGui::Button("Cancel"))
        {
            memset(Name, 0, sizeof(Name));
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }
}

void CImguiMgr::CreateSpritePopup()
{
    if (m_PopupFlag & PopupFlags_CreateSprite)
    {
        m_PopupFlag ^= PopupFlags_CreateSprite;
        ImGui::OpenPopup("CreateSprite");
    }

    if (ImGui::BeginPopupModal("CreateSprite", NULL, ImGuiWindowFlags_AlwaysAutoResize))
    {
        float tab = 130.f;
        ImGui::Text("Create new sprite.");
        ImGui::NewLine();

        // 오브젝트 이름 입력
        ImGui::Text("Name: ");
        ImGui::SameLine(tab);
        static char Name[255] = {};
        ImGui::InputText("##Name", Name, 255);
        std::wstring WName = to_wstr(std::string(Name));

        if (ImGui::Button("Create"))
        {
            CSprite::Create(WName);
            memset(Name, 0, sizeof(Name));
            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();
        if (ImGui::Button("Cancel"))
        {
            memset(Name, 0, sizeof(Name));
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }
}

void CImguiMgr::LoadAsset()
{
    // 에셋 파일 열기
    WCHAR filepath[255] = {};
    WCHAR filename[255] = {};
    wstring ContentDir = CPathMgr::GetContentDir();
    OPENFILENAME Desc = {};
    Desc.lStructSize = sizeof(OPENFILENAME);
    Desc.hwndOwner = CEngine::GetInst()->GetMainWndHwnd();
    Desc.lpstrFilter = L"ALL\0*.*\0";
    Desc.lpstrFile = filepath;
    Desc.nMaxFile = 255;
    Desc.lpstrFileTitle = filename;
    Desc.nMaxFileTitle = 255;
    Desc.lpstrInitialDir = ContentDir.c_str();
    Desc.lpstrTitle = L"에셋 파일 로드";
    Desc.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;

    CURSOR_ON;
    if (GetOpenFileName(&Desc))
    {
        CURSOR_OFF;
        std::wstring path = filepath;
        if (path.find(CPathMgr::GetContentDir()) == std::wstring::npos)
        {
            MessageBoxW(nullptr, L"파일 경로가 Content 디렉토리가 아닙니다.", L"Asset Load Error", MB_OK);
            return;
        }
        std::filesystem::path RelativePath = path.substr(ContentDir.size());
        std::filesystem::path EXT = RelativePath.extension();
        if (EXT == ".mesh")
            CAssetMgr::GetInst()->Load<CMesh>(RelativePath);
        else if (EXT == ".mtrl")
            CAssetMgr::GetInst()->Load<CMaterial>(RelativePath);
        else if (EXT == L".dds" || EXT == L".DDS"
                || EXT == L".tga" || EXT == L".TGA"
                || EXT == L".png" || EXT == L".PNG"
                || EXT == L".jpg" || EXT == L".JPG"
                || EXT == L".jpeg" || EXT == L".JPEG"
                || EXT == L".bmp" || EXT == L".BMP")
            CAssetMgr::GetInst()->Load<CTexture2D>(RelativePath);
        else if (EXT == L".wav" || EXT == L".WAV"
                || EXT == L".mp3" || EXT == L".MP3"
                || EXT == L".aac" || EXT == L".AAC"
                || EXT == L".ogg" || EXT == L".OGG"
                || EXT == L".flac" || EXT == L".FLAC"
                || EXT == L".wma" || EXT == L".WMA")
            CAssetMgr::GetInst()->Load<CSound>(RelativePath);
        else if (EXT == ".shader")
            CAssetMgr::GetInst()->Load<CGraphicShader>(RelativePath);
        else if (EXT == ".prefab")
            CAssetMgr::GetInst()->Load<CPrefab>(RelativePath);
        else if (EXT == ".sprite")
            CAssetMgr::GetInst()->Load<CSprite>(RelativePath);
        else if (EXT == ".flip")
            CAssetMgr::GetInst()->Load<CFlipbook>(RelativePath);
        else
            MessageBoxW(nullptr, L"지원하지 않는 파일 형식입니다.", L"Asset Load Error", MB_OK);
    }
    CURSOR_OFF;
}

void CImguiMgr::LoadLevel()
{
    // 에셋 파일 열기
    WCHAR filepath[255] = {};
    WCHAR filename[255] = {};
    wstring ContentDir = CPathMgr::GetContentDir();
    OPENFILENAME Desc = {};
    ZeroMemory(&Desc, sizeof(Desc));
    Desc.lStructSize = sizeof(OPENFILENAME);
    Desc.hwndOwner = CEngine::GetInst()->GetMainWndHwnd();
    Desc.lpstrFilter = L"ALL\0*.*\0\0";
    Desc.lpstrFile = filepath;
    Desc.nMaxFile = 255;
    Desc.lpstrFileTitle = filename;
    Desc.nMaxFileTitle = 255;
    Desc.lpstrInitialDir = ContentDir.c_str();
    Desc.lpstrTitle = L"레벨 파일 로드";
    Desc.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;

    CURSOR_ON;
    if (GetOpenFileName(&Desc))
    {
        CURSOR_OFF;
        std::wstring path = filepath;
        if (path.find(CPathMgr::GetContentDir()) == std::wstring::npos)
        {
            MessageBoxW(nullptr, L"파일 경로가 Content 디렉토리가 아닙니다.", L"Level Load Error", MB_OK);
            return;
        }
        std::filesystem::path RelativePath = path.substr(ContentDir.size());
        std::filesystem::path EXT = RelativePath.extension();
        if (EXT == ".level")
        {
            CLevel* NewLevel = CLevelMgr::GetInst()->FindLevel(RelativePath);
            CTaskMgr::GetInst()->AddTask(TASK_TYPE::CHANGE_LEVEL, reinterpret_cast<DWORD_PTR>(NewLevel), (DWORD_PTR)false);
        }
        else
            MessageBoxW(nullptr, L"지원하지 않는 파일 형식입니다.", L"Asset Load Error", MB_OK);
    }
    CURSOR_OFF;
}