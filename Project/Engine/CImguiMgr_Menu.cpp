#include "pch.h"
#include "CImguiMgr.h"

#include "CTaskMgr.h"
#include "CLevelMgr.h"
#include "CTimeMgr.h"
#include "CRenderMgr.h"
#include "CAssetMgr.h"

#include "CEngine.h"
#include "CLevel.h"
#include "CInspector.h"
#include "COutliner.h"
#include "CContentViewer.h"
#include "CMaterial.h"
#include "CGameObject.h"
#include "CTransform.h"

void CImguiMgr::DebugMenuBar()
{
    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::BeginMenu("Asset"))
            {
                if (ImGui::MenuItem("Asset Load", nullptr))
                {
                    LoadAsset();
                }

                if (ImGui::MenuItem("Asset Update", nullptr))
                {
                    CAssetMgr::GetInst()->UpdateAsset();
                }

				if (ImGui::BeginMenu("Asset Create"))
				{
                    if (ImGui::MenuItem("Material", nullptr))
                    {
                        m_PopupFlag |= PopupFlags_CreateMaterial;
                    }
                    if (ImGui::MenuItem("Flipbook", nullptr))
                    {
                        m_PopupFlag |= PopupFlags_CreateFlipbook;
                    }
                    if (ImGui::MenuItem("Sprite", nullptr))
                    {
                        m_PopupFlag |= PopupFlags_CreateSprite;
                    }
                    if (ImGui::MenuItem("Graphic Shader", nullptr))
                    {

                    }

                    ImGui::EndMenu();
				}

                if (ImGui::MenuItem("Load FBX", nullptr))
                {
                    LoadFBX();
                }

				ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Level"))
            {

                if (ImGui::MenuItem("New", nullptr))
                {
                    m_PopupFlag |= PopupFlags_NewLevel;
                }
                if (ImGui::MenuItem("Save", nullptr))
                {
                    CLevel* Level = CLevelMgr::GetInst()->GetCurrentLevel();
                    if (Level) { Level->Save(Level->GetName()); }
                }
                if (ImGui::MenuItem("Save As..", nullptr))
                {
                    m_PopupFlag |= PopupFlags_SaveLevel;
                }
                if (ImGui::MenuItem("Load", nullptr))
                {
                    LoadLevel();
                }

                ImGui::EndMenu();
            }
            

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Edit"))
        {
            if (ImGui::BeginMenu("Level"))
            {
                if (ImGui::MenuItem("Add GameObject"))
                {
                    m_PopupFlag |= PopupFlags_AddGameObjectMenu;
                }

                if (ImGui::BeginMenu("Change State"))
                {
                    LEVEL_STATE state = LEVEL_STATE::NONE;
                    if (CLevelMgr::GetInst()->GetCurrentLevel())
                        state = CLevelMgr::GetInst()->GetCurrentLevel()->GetState();
                    if (ImGui::MenuItem("Play", nullptr, state == LEVEL_STATE::PLAY, state != LEVEL_STATE::NONE))
                    {
                        CTaskMgr::GetInst()->AddTask(TASK_TYPE::CHANGE_LEVEL_STATE, static_cast<DWORD_PTR>(LEVEL_STATE::PLAY), NULL);
                    }
                    if (ImGui::MenuItem("Stop", nullptr, state == LEVEL_STATE::STOP, state != LEVEL_STATE::NONE))
                    {
                        CTaskMgr::GetInst()->AddTask(TASK_TYPE::CHANGE_LEVEL_STATE, static_cast<DWORD_PTR>(LEVEL_STATE::STOP), NULL);
                    }
                    if (ImGui::MenuItem("Pause", nullptr, state == LEVEL_STATE::PAUSE, state != LEVEL_STATE::NONE))
                    {
                        CTaskMgr::GetInst()->AddTask(TASK_TYPE::CHANGE_LEVEL_STATE, static_cast<DWORD_PTR>(LEVEL_STATE::PAUSE), NULL);
                    }

                    ImGui::EndMenu();
                }

                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Camera"))
            {
                if (ImGui::MenuItem("Position Initialize"))
                {
                    if (CGameObject* pGO = CRenderMgr::GetInst()->GetEditorCam(); pGO)
                    {
                        if (CTransform* pTrans = pGO->Transform(); pTrans)
                        {
                            pTrans->SetRelativePos(Vec3(0, 0, 0));
                            pTrans->SetRelativeRot(Vec4(0.f, 0.f, 0.f, 1.f));
                        }
                    }
                }
                ImGui::EndMenu();
            }

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Tool"))
        {
            ImGui::MenuItem("Demo Window", nullptr, &m_DemoActive);

            if (ImGui::MenuItem("Sound Unbind"))
            {
                FMOD::ChannelGroup* master = nullptr;
                CEngine::GetInst()->GetFMODSystem()->getMasterChannelGroup(&master);
                master->stop();
            }

            if (ImGui::MenuItem("Reset EditorCam"))
            {
                CRenderMgr::GetInst()->ResetEditorCamPos();
            }

            if (ImGui::MenuItem("Inspector"))
            {
                m_Inspector->SetActive(true);
            }

            if (ImGui::MenuItem("Outliner"))
            {
                m_Outliner->SetActive(true);
            }

            if (ImGui::MenuItem("Content Viewer"))
            {
                m_ContentViewer->SetActive(true);
            }

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Render"))
        {
            if (ImGui::BeginMenu("Merge Mode"))
            {
                if (ImGui::MenuItem("Default"))
                {
                    CRenderMgr::GetInst()->SetMergeMode(0);
                }
                if (ImGui::MenuItem("Albedo"))
                {
                    CRenderMgr::GetInst()->SetMergeMode(1);
                }
                if (ImGui::MenuItem("Normal"))
                {
                    CRenderMgr::GetInst()->SetMergeMode(2);
                }
                if (ImGui::MenuItem("Position"))
                {
                    CRenderMgr::GetInst()->SetMergeMode(3);
                }
                if (ImGui::MenuItem("Effect"))
                {
                    CRenderMgr::GetInst()->SetMergeMode(4);
                }
                if (ImGui::MenuItem("Diffuse"))
                {
                    CRenderMgr::GetInst()->SetMergeMode(5);
                }
                if (ImGui::MenuItem("Specular"))
                {
                    CRenderMgr::GetInst()->SetMergeMode(6);
                }
                if (ImGui::MenuItem("Emissive"))
                {
                    CRenderMgr::GetInst()->SetMergeMode(7);
                }

                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("DebugOption"))
            {
                if (ImGui::MenuItem("Default"))
                {
                    CRenderMgr::GetInst()->SetDebugOption(0);
                }
                if (ImGui::MenuItem("Camera Frustum Section"))
                {
                    CRenderMgr::GetInst()->SetDebugOption(1);
                }

                ImGui::EndMenu();
            }

            ImGui::EndMenu();
        }

        ImGui::EndMainMenuBar();
    }


    // 팝업함수 호출
    AddGameObjectMenuPopup();
    NewLevelPopup();
    SaveLevelPopup();
    CreateMaterialPopup();
    CreateFlipbookPopup();
    CreateSpritePopup();
}