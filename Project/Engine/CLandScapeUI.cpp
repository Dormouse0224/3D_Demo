#include "pch.h"
#include "CLandScapeUI.h"

#include "CAssetMgr.h"

#include "CGameObject.h"
#include "CLandScape.h"
#include "CDevice.h"

CLandScapeUI::CLandScapeUI()
    : CRenderComponentUI(COMPONENT_TYPE::LANDSCAPE)
{
}

CLandScapeUI::~CLandScapeUI()
{
}

void CLandScapeUI::Update_Com()
{
}

void CLandScapeUI::Render_Com()
{
    Render_RCom();

    ImGui::NewLine();
    ImGui::SeparatorText("LandScape Render Option");
    CLandScape* pLandScape = m_TargetObj->LandScape();
    
    ImGui::Text("Grid Info");
    int grid[2] = { pLandScape->GetFaceX(), pLandScape->GetFaceZ() };
    if (ImGui::DragInt2("##GridInfo", grid, 1.f, 1, 10))
    {
        grid[0] = min(max(grid[0], 1), 10);
        grid[1] = min(max(grid[1], 1), 10);
        pLandScape->SetFace(grid[0], grid[1]);
    }

    ImGui::NewLine();
    ImGui::Text("Color Texture");
    std::string ColorTexName = "NO EXIST";
    if (pLandScape->GetColorTexture().Get())
    {
        ColorTexName = to_str(pLandScape->GetColorTexture()->GetName());
    }
    ImGui::InputText("##ColorTexture", const_cast<char*>(ColorTexName.c_str())
        , ColorTexName.size() + 1, ImGuiInputTextFlags_ReadOnly);
    if (ImGui::BeginDragDropTarget())
    {
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ContentViewer"))
        {
            CAsset* Asset = *reinterpret_cast<CAsset**>(payload->Data);
            AssetPtr<CTexture> pTex = dynamic_cast<CTexture*>(Asset);
            if (pTex.Get())
            {
                pLandScape->SetColorTexture(pTex);
            }
        }
        ImGui::EndDragDropTarget();
    }

    ImGui::NewLine();
    ImGui::Text("Normal Texture");
    std::string NormalTexName = "NO EXIST";
    if (pLandScape->GetNormalTexture().Get())
    {
        NormalTexName = to_str(pLandScape->GetNormalTexture()->GetName());
    }
    ImGui::InputText("##NormalTexture", const_cast<char*>(NormalTexName.c_str())
        , NormalTexName.size() + 1, ImGuiInputTextFlags_ReadOnly);
    if (ImGui::BeginDragDropTarget())
    {
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ContentViewer"))
        {
            CAsset* Asset = *reinterpret_cast<CAsset**>(payload->Data);
            AssetPtr<CTexture> pTex = dynamic_cast<CTexture*>(Asset);
            if (pTex.Get())
            {
                pLandScape->SetNormalTexture(pTex);
            }
        }
        ImGui::EndDragDropTarget();
    }

    ImGui::NewLine();
    ImGui::Text("Edit Mode");
    const char* editModes[] = { "NONE", "HEIGHTMAP", "SPLATING" };
    int editMode = (int)pLandScape->GetMode();
    if (ImGui::BeginCombo("##EditMode", editModes[editMode]))
    {
        for (int n = 0; n < IM_ARRAYSIZE(editModes); n++)
        {
            const bool is_selected = (editMode == n);
            if (ImGui::Selectable(editModes[n], is_selected))
            {
                pLandScape->SetMode((LANDSCAPE_MODE)n);
            }

            // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
            if (is_selected)
                ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
    }

    ImGui::NewLine();
    ImGui::Text("Brush Type");
    int brushIdx = pLandScape->GetBrushIdx();
    if (ImGui::SliderInt("##BrushType", &brushIdx, 0, pLandScape->GetBrushes().size() - 1))
    {
        pLandScape->SetBrushIdx(brushIdx);
    }

    if (!pLandScape->GetBrushes().empty() && pLandScape->GetBrushes().size() > pLandScape->GetBrushIdx())
    {
        float ImageWidth = 320.f;
        AssetPtr<CTexture> pTex = pLandScape->GetBrushes()[pLandScape->GetBrushIdx()];
        Vec2 origin(ImGui::GetCursorScreenPos().x, ImGui::GetCursorScreenPos().y);
        ImGui::Image(reinterpret_cast<ImTextureID>(pTex->GetSRV().Get())
            , ImVec2(ImageWidth, ImageWidth * (static_cast<float>(pTex->GetHeight()) / static_cast<float>(pTex->GetWidth())))
            , ImVec2(0, 0), ImVec2(1, 1), ImVec4(1, 1, 1, 1), ImVec4(1, 1, 1, 1));
        float scale = ImageWidth / static_cast<float>(pTex->GetWidth());
    }


    ImGui::NewLine();
    ImGui::Text("Brush Texture");
    int weightIdx = pLandScape->GetWeightIdx();
    if (pLandScape->GetColorTexture().Get())
    {
        if (ImGui::SliderInt("##BrushTexture", &weightIdx, 0, pLandScape->GetColorTexture()->GetDesc().ArraySize - 1))
        {
            pLandScape->SetWeightIdx(weightIdx);
        }

        //if (AssetPtr<CTexture> pTex = pLandScape->GetColorTexture(); pTex.Get())
        //{
        //    float ImageWidth = 320.f;

        //    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
        //    srvDesc.Format = pTex->GetDesc().Format;
        //    if (pTex->GetDesc().ArraySize > 1)
        //    {
        //        srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
        //        srvDesc.Texture2DArray.FirstArraySlice = pLandScape->GetWeightIdx();
        //        srvDesc.Texture2DArray.ArraySize = 1;
        //        srvDesc.Texture2D.MostDetailedMip = 0;
        //        srvDesc.Texture2D.MipLevels = -1;
        //    }
        //    else
        //    {
        //        srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
        //        srvDesc.Texture2D.MostDetailedMip = 0;
        //        srvDesc.Texture2D.MipLevels = -1;
        //    }
        //    ComPtr<ID3D11ShaderResourceView> pSRV = nullptr;
        //    if(!FAILED(DEVICE->CreateShaderResourceView(pTex->GetTex().Get(), &srvDesc, pSRV.GetAddressOf())))
        //    {
        //        Vec2 origin(ImGui::GetCursorScreenPos().x, ImGui::GetCursorScreenPos().y);
        //        ImGui::Image(reinterpret_cast<ImTextureID>(pSRV.Get())
        //            , ImVec2(ImageWidth, ImageWidth * (static_cast<float>(pTex->GetHeight()) / static_cast<float>(pTex->GetWidth())))
        //            , ImVec2(0, 0), ImVec2(1, 1), ImVec4(1, 1, 1, 1), ImVec4(1, 1, 1, 1));
        //        float scale = ImageWidth / static_cast<float>(pTex->GetWidth());
        //    }
        //}
    }
    else
        ImGui::Text("Texture Not Exist");
}
