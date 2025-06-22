#include "pch.h"
#include "CParticleRenderUI.h"
#include "CParticleRender.h"

CParticleRenderUI::CParticleRenderUI()
	: CRenderComponentUI(COMPONENT_TYPE::PARTICLERENDER)
{
}

CParticleRenderUI::~CParticleRenderUI()
{
}

void CParticleRenderUI::Update_Com()
{
}

void CParticleRenderUI::Render_Com()
{
    void Render_RCom();

    ImGui::SeparatorText("Particle Render Option");
    UINT& MaxParticle = static_cast<CParticleRender*>(m_TargetObj->GetRenderComponent())->m_MaxParticle;
    int& SpawnCount = static_cast<CParticleRender*>(m_TargetObj->GetRenderComponent())->m_SpawnCount;
    bool& Active = static_cast<CParticleRender*>(m_TargetObj->GetRenderComponent())->m_Active;
    bool& Gravity = static_cast<CParticleRender*>(m_TargetObj->GetRenderComponent())->m_Gravity;

    ImGui::Text("Max Particle");
    ImGui::SameLine();
    if (ImGui::InputScalar("##MaxParticle", ImGuiDataType_U32, &MaxParticle));

    ImGui::Text("SpawnCount");
    ImGui::SameLine();
    if (ImGui::InputInt("##SpawnCount", &SpawnCount));

    ImGui::Text("Active?");
    ImGui::SameLine();
    ImGui::Checkbox("##Active", &Active);

    ImGui::Text("Gravity?");
    ImGui::SameLine();
    ImGui::Checkbox("##Gravity", &Gravity);

    ImGui::SeparatorText("Particle Module Option");
    tParticleModule& Module = static_cast<CParticleRender*>(m_TargetObj->GetRenderComponent())->m_Module;

    ImGui::Text("Spawn Rate");
    ImGui::SameLine();
    ImGui::InputFloat("##SpawnRate", &Module.SpawnRate);

    ImGui::Text("Start Color");
    ImGui::SameLine();
    float farrStartColor[] = { Module.StartColor.x, Module.StartColor.y, Module.StartColor.z, Module.StartColor.w };
    if (ImGui::InputFloat4("##StartColor", farrStartColor))
    {
        Module.StartColor.x = farrStartColor[0];
        Module.StartColor.y = farrStartColor[1];
        Module.StartColor.z = farrStartColor[2];
        Module.StartColor.w = farrStartColor[3];
    }

    ImGui::Text("End Color");
    ImGui::SameLine();
    float farrEndColor[] = { Module.EndColor.x, Module.EndColor.y, Module.EndColor.z, Module.EndColor.w };
    if (ImGui::InputFloat4("##EndColor", farrEndColor))
    {
        Module.EndColor.x = farrEndColor[0];
        Module.EndColor.y = farrEndColor[1];
        Module.EndColor.z = farrEndColor[2];
        Module.EndColor.w = farrEndColor[3];
    }

    ImGui::Text("Minimum Scale");
    ImGui::SameLine();
    float farrMinScale[] = { Module.MinScale.x, Module.MinScale.y, Module.MinScale.z };
    if (ImGui::InputFloat3("##MinScale", farrMinScale))
    {
        Module.MinScale.x = min(farrMinScale[0], Module.MaxScale.x);
        Module.MinScale.y = min(farrMinScale[1], Module.MaxScale.y);
        Module.MinScale.z = min(farrMinScale[2], Module.MaxScale.z);
    }

    ImGui::Text("Maximum Scale");
    ImGui::SameLine();
    float farrMaxScale[] = { Module.MaxScale.x, Module.MaxScale.y, Module.MaxScale.z };
    if (ImGui::InputFloat3("##MaxScale", farrMaxScale))
    {
        Module.MaxScale.x = max(farrMaxScale[0], Module.MinScale.x);
        Module.MaxScale.y = max(farrMaxScale[1], Module.MinScale.y);
        Module.MaxScale.z = max(farrMaxScale[2], Module.MinScale.z);
    }

    ImGui::Text("Minimum Speed");
    ImGui::SameLine();
    float fMinSpeed = Module.MinSpeed;
    if (ImGui::InputFloat("##MinSpeed", &fMinSpeed))
    {
        Module.MinSpeed = min(fMinSpeed, Module.MaxSpeed);
    }

    ImGui::Text("Maximum Speed");
    ImGui::SameLine();
    float fMaxSpeed = Module.MaxSpeed;
    if (ImGui::InputFloat("##MaxSpeed", &fMaxSpeed))
    {
        Module.MaxSpeed = max(fMaxSpeed, Module.MinSpeed);
    }

    ImGui::Text("Minimum Life");
    ImGui::SameLine();
    float fMinLife = Module.MinLife;
    if (ImGui::InputFloat("##MinLife", &fMinLife))
    {
        Module.MinLife = min(fMinLife, Module.MaxLife);
    }

    ImGui::Text("Maximum Life");
    ImGui::SameLine();
    float fMaxLife = Module.MaxLife;
    if (ImGui::InputFloat("##MaxLife", &fMaxLife))
    {
        Module.MaxLife = max(fMaxLife, Module.MinLife);
    }

    ImGui::Text("Spawn Shape");
    const char* shapes[2] = { "Box", "Sphere" };
    ImGui::Combo("##SpawnShape", &Module.SpawnShape, shapes, 2);

    ImGui::Text("Spawn Shape Scale");
    ImGui::SameLine();
    float farrSpawnShapeScale[] = { Module.SpawnShapeScale.x, Module.SpawnShapeScale.y, Module.SpawnShapeScale.z };
    if (ImGui::InputFloat3("##SpawnShapeScale", farrSpawnShapeScale))
    {
        Module.SpawnShapeScale.x = farrSpawnShapeScale[0];
        Module.SpawnShapeScale.y = farrSpawnShapeScale[1];
        Module.SpawnShapeScale.z = farrSpawnShapeScale[2];
    }

    ImGui::Text("Spawn Direction");
    ImGui::SameLine();
    float farrSpawnDir[] = { Module.SpawnDir.x, Module.SpawnDir.y, Module.SpawnDir.z };
    if (ImGui::InputFloat3("##SpawnDirection", farrSpawnDir))
    {
        Module.SpawnDir.x = farrSpawnDir[0];
        Module.SpawnDir.y = farrSpawnDir[1];
        Module.SpawnDir.z = farrSpawnDir[2];
    }

    ImGui::Text("Spawn Direction Randomize Rate");
    ImGui::SameLine();
    ImGui::SliderFloat("##SpawnDirRndRate", &Module.SpawnDirRandomize, 0, 1);

    ImGui::Text("Space Type");
    const char* spacetype[2] = { "Loacl", "World" };
    ImGui::Combo("##SpaceType", &Module.SpaceType, spacetype, 2);

    ImGui::Text("Velocity Allignment?");
    ImGui::SameLine();
    bool bVA = !!Module.VelocityAllignment;
    if (ImGui::Checkbox("##VelocityAllignment", &bVA))
    {
        Module.VelocityAllignment = bVA;
    }

    ImGui::SeparatorText("Particle Data");
    AssetPtr<CTexture2D> ParticleTex = static_cast<CParticleRender*>(m_TargetObj->GetRenderComponent())->m_ParticleTex;
    map<UINT, CGameObject*>& mapParticleObj = static_cast<CParticleRender*>(m_TargetObj->GetRenderComponent())->m_mapParticleObj;

    ImGui::Text("Particle Texture");
    std::string ParticleTexName = "NO EXIST";
    if (ParticleTex.Get())
    {
        ParticleTexName = to_str(ParticleTex->GetName());
    }
    ImGui::InputText("##ParticleTex", const_cast<char*>(ParticleTexName.c_str())
        , ParticleTexName.size() + 1, ImGuiInputTextFlags_ReadOnly);
    if (ImGui::BeginDragDropTarget())
    {
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ContentViewer"))
        {
            CAsset* Asset = *reinterpret_cast<CAsset**>(payload->Data);
            AssetPtr<CTexture2D> pTex = dynamic_cast<CTexture2D*>(Asset);
            if (pTex.Get())
            {
                m_TargetObj->ParticleRender()->SetParticleTex(pTex);
            }
        }
        ImGui::EndDragDropTarget();
    }

    ImGui::Text("Current Activate Particle Count : %d", mapParticleObj.size());
}
