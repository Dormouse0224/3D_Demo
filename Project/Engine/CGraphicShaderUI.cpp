#include "pch.h"
#include "CGraphicShaderUI.h"

#include "CGraphicShader.h"

CGraphicShaderUI::CGraphicShaderUI()
	: CAssetUI(ASSET_TYPE_WSTR[(UINT)ASSET_TYPE::GRAPHIC_SHADER])
{
}

CGraphicShaderUI::~CGraphicShaderUI()
{
}

void CGraphicShaderUI::Update_Ast()
{
}

void CGraphicShaderUI::Render_Ast()
{
    AssetPtr<CGraphicShader> pGS = static_cast<CGraphicShader*>(m_TargetAsset.Get());
    float tab = 130;

    // 그래픽 셰이더의 셰이더 정보
    for (int i = 0; i < SHADER_TYPE_END; ++i)
    {
        if (!pGS->GetShaderName(static_cast<SHADER_TYPE>(i)).first.empty())
        {
            ImGui::Text(SHADER_TYPE_STR[i].c_str());
            string label0 = "##ShaderDir" + to_string(i);
            string label1 = "##ShaderFunc" + to_string(i);
            ImGui::InputText(label0.c_str(), const_cast<char*>(to_str(pGS->GetShaderName(static_cast<SHADER_TYPE>(i)).first).c_str())
                , to_str(pGS->GetShaderName(static_cast<SHADER_TYPE>(i)).first).size() + 1, ImGuiInputTextFlags_ReadOnly);
            ImGui::InputText(label1.c_str(), const_cast<char*>(pGS->GetShaderName(static_cast<SHADER_TYPE>(i)).second.c_str())
                , pGS->GetShaderName(static_cast<SHADER_TYPE>(i)).second.size() + 1, ImGuiInputTextFlags_ReadOnly);
        }
    }
    
    // 토폴로지 정보
    ImGui::Text("Topology");
    const char* topology[] = 
    {
        "D3D11_PRIMITIVE_TOPOLOGY_UNDEFINED",           // 0
        "D3D11_PRIMITIVE_TOPOLOGY_POINTLIST",           // 1
        "D3D11_PRIMITIVE_TOPOLOGY_LINELIST",            // 2
        "D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP",           // 3
        "D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST",        // 4
        "D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP",       // 5
        "D3D11_PRIMITIVE_TOPOLOGY_LINELIST_ADJ",        // 10
        "D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP_ADJ",       // 11
        "D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST_ADJ",    // 12
        "D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP_ADJ",   // 13
    };
    int topology_current = pGS->GetTopology();
    if (topology_current > 5)
        topology_current -= 4;
    if (topology_current < IM_ARRAYSIZE(topology))
        if (ImGui::Combo("##Topology", &topology_current, topology, IM_ARRAYSIZE(topology)))
        {
            if (topology_current > 5)
                topology_current += 4;
            pGS->SetTopology(static_cast<D3D11_PRIMITIVE_TOPOLOGY>(topology_current));
        }
    else
        ImGui::Text("Topology out of range");
   

    // 셰이더 렌더링 도메인
    ImGui::Text("Render Domain");
    const char* domain[] = { "DOMAIN_QPAQUE", "DOMAIN_MASKED", "DOMAIN_TRANSPARENT", "DOMAIN_POSTPROCESS", "DOMAIN_UI", "DOMAIN_DEBUG" };
    int domain_current = static_cast<int>(pGS->GetDomain());
    if (ImGui::Combo("##RenderDomain", &domain_current, domain, IM_ARRAYSIZE(domain)))
    {
        pGS->SetDomain(static_cast<SHADER_DOMAIN>(domain_current));
    }

    // 래스터라이저 설정
    ImGui::Text("Resterizer State");
    const char* RSType[] = { "CULL_BACK", "CULL_FRONT", "CULL_NONE", "WIRE_FRAME" };
    int RSType_current = static_cast<int>(pGS->GetRSType());
    if (ImGui::Combo("##ResterizerState", &RSType_current, RSType, IM_ARRAYSIZE(RSType)))
    {
        pGS->SetRSType(static_cast<RS_TYPE>(RSType_current));
    }

    // 깊이 스텐실 설정
    ImGui::Text("Depth-Stencil State");
    const char* DSType[] = { "LESS", "LESS_EQUAL", "GREATER", "NO_WRITE", "NO_TEST_NO_WIRITE" };
    int DSType_current = static_cast<int>(pGS->GetDSType());
    if (ImGui::Combo("##DepthStencilState", &DSType_current, DSType, IM_ARRAYSIZE(DSType)))
    {
        pGS->SetDSType(static_cast<DS_TYPE>(DSType_current));
    }

    // 혼합 설정
    ImGui::Text("Blend State");
    const char* BSType[] = { "DEFAULT", "ALPHABLEND", "ALPHABLEND_COVERAGE", "ONE_ONE" };
    int BSType_current = static_cast<int>(pGS->GetDomain());
    if (ImGui::Combo("##BlendState", &BSType_current, BSType, IM_ARRAYSIZE(BSType)))
    {
        pGS->SetBSType(static_cast<BS_TYPE>(BSType_current));
    }
}
