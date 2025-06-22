#include "pch.h"
#include "CPrefabUI.h"

#include "CTaskMgr.h"

#include "CPrefab.h"
#include "CGameObject.h"
#include "CScript.h"

CPrefabUI::CPrefabUI()
	: CAssetUI(ASSET_TYPE_WSTR[(UINT)ASSET_TYPE::PREFAB])
{
}

CPrefabUI::~CPrefabUI()
{
}

void CPrefabUI::Update_Ast()
{
}

void CPrefabUI::Render_Ast()
{
    AssetPtr<CPrefab> pPrefab = static_cast<CPrefab*>(m_TargetAsset.Get());
    ObjectInfo(pPrefab->GetPrefabObject());

    ImGui::NewLine();
    if (ImGui::Button("Instanciate"))
    {
        ImGui::OpenPopup("InstanciatePopup");
    }
    if (ImGui::BeginPopup("InstanciatePopup"))
    {
        ImGui::Text("Add prefab as new GameObject to current level.");
        ImGui::NewLine();

        // 오브젝트 이름 입력
        ImGui::Text("Name: ");
        ImGui::SameLine();
        static char Name[255] = {};
        ImGui::InputText("##Name", Name, 255);
        std::wstring WName = to_wstr(std::string(Name));

        if (ImGui::Button("Add"))
        {
            CGameObject* pObj = pPrefab->Instantiate();
            pObj->SetName(WName);
            CTaskMgr::GetInst()->AddTask(TASK_TYPE::CREATE_OBJECT, reinterpret_cast<DWORD_PTR>(pObj), static_cast<DWORD_PTR>(pObj->GetLayerIdx()));
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

void CPrefabUI::ObjectInfo(CGameObject* _Obj)
{
    std::string ObjName = to_str(_Obj->GetName());
    std::string uiid = ObjName + "_" + to_string(_Obj->GetID());
    ImGui::SeparatorText("Prefab Object Info");


    ImGui::Text("Name");
    uiid = "##Name_" + to_string(_Obj->GetID());
    ImGui::InputText(uiid.c_str(), const_cast<char*>(ObjName.c_str())
        , ObjName.size() + 1, ImGuiInputTextFlags_ReadOnly);

    ImGui::NewLine();
    ImGui::Text("Layer Index");
    uiid = "##LayerIdx" + to_string(_Obj->GetID());
    int LayerIdx = _Obj->GetLayerIdx();
    ImGui::InputInt(uiid.c_str(), &LayerIdx, 1, 100, ImGuiInputTextFlags_ReadOnly);


    ImGui::NewLine();
    uiid = "ComponentList##" + to_string(_Obj->GetID());
    if (ImGui::BeginTable(uiid.c_str(), 1, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg))
    {
        ImGui::TableSetupColumn("Component List");
        ImGui::TableHeadersRow();

        for (int i = 0; i < (UINT)COMPONENT_TYPE::COMPONENT_END; ++i)
        {
            if (_Obj->GetComponent(static_cast<COMPONENT_TYPE>(i)))
            {
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::TextUnformatted(to_str(COMPONENT_TYPE_WSTR[i]).c_str());
            }
        }
        ImGui::EndTable();
    }

    ImGui::NewLine();
    const auto& vecScript = _Obj->GetScripts();
    int count = vecScript.size();
    uiid = "ScriptList##" + to_string(_Obj->GetID());
    if (ImGui::BeginTable(uiid.c_str(), 1, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg))
    {
        ImGui::TableSetupColumn("Script List");
        ImGui::TableHeadersRow();

        for (int i = 0; i < count; ++i)
        {
            if (_Obj->GetComponent(static_cast<COMPONENT_TYPE>(i)))
            {
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                std::string ClassName = typeid(*vecScript[i]).name();
                ImGui::TextUnformatted(ClassName.c_str());
            }
        }
        ImGui::EndTable();
    }

    uiid = "Show Child##" + to_string(_Obj->GetID());
    if (ImGui::Button(uiid.c_str()))
    {
        ImGui::OpenPopup("childs");
    }

    if (ImGui::BeginPopup("childs"))
    {
        for (CGameObject* child : _Obj->GetChild())
            ObjectInfo(child);
        if (ImGui::Button("close"))
        {
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }
    //CGameObject* m_Parent;   // 부모 오브젝트
    
}
