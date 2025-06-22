#include "pch.h"
#include "CComputeShaderUI.h"

#include "CComputeShader.h"

CComputeShaderUI::CComputeShaderUI()
	: CAssetUI(ASSET_TYPE_WSTR[(UINT)ASSET_TYPE::COMPUTE_SHADER])
{
}

CComputeShaderUI::~CComputeShaderUI()
{
}

void CComputeShaderUI::Update_Ast()
{
}

void CComputeShaderUI::Render_Ast()
{
    AssetPtr<CComputeShader> pCS = static_cast<CComputeShader*>(m_TargetAsset.Get());
	float tab = 150.f;

	// 전달된 상수 데이터
	vector<tConstData*> vecConstData = pCS->GetConstData();
	tMtrlConst Const = pCS->GetConstParam();
	for (tConstData* data : vecConstData)
	{
		ImGui::Text(data->m_Desc.c_str());
		ImGui::SameLine(tab);
		string label = "##ConstData" + to_string(data->m_Param);
		switch (data->m_Param)
		{
		case INT_0:
		case INT_1:
		case INT_2:
		case INT_3:
		{
			ImGui::InputInt(label.c_str(), &Const.iArr[data->m_Param - INT_0], 1, 100, ImGuiInputTextFlags_ReadOnly);
		}
		break;
		case FLOAT_0:
		case FLOAT_1:
		case FLOAT_2:
		case FLOAT_3:
		{
			ImGui::InputFloat(label.c_str(), &Const.fArr[data->m_Param - FLOAT_0], 0, 0, "%.3f", ImGuiInputTextFlags_ReadOnly);
		}
		break;
		case VEC2_0:
		case VEC2_1:
		case VEC2_2:
		case VEC2_3:
		{
			float vec[2] = { Const.v2Arr[data->m_Param - VEC2_0].x, Const.v2Arr[data->m_Param - VEC2_0].y };
			ImGui::InputFloat2(label.c_str(), vec, "%.3f", ImGuiInputTextFlags_ReadOnly);
		}
		break;
		case VEC4_0:
		case VEC4_1:
		case VEC4_2:
		case VEC4_3:
		{
			Vec4 v4 = Const.v4Arr[data->m_Param - VEC4_0];
			float vec[4] = { v4.x, v4.y, v4.z, v4.w };
			ImGui::InputFloat4(label.c_str(), vec, "%.3f", ImGuiInputTextFlags_ReadOnly);
		}
		break;
		case MAT_0:
		case MAT_1:
		case MAT_2:
		case MAT_3:
		{
			if (ImGui::BeginTable(label.c_str(), 4, ImGuiTableFlags_NoSavedSettings | ImGuiTableFlags_Borders))
			{
				Matrix mat = Const.matArr[data->m_Param - MAT_0];
				for (int row = 0; row < 4; ++row)
				{
					ImGui::TableNextRow();
					for (int col = 0; col < 4; ++col)
					{
						ImGui::TableSetColumnIndex(col);
						string tablelabel = label + to_string(row) + to_string(col);
						ImGui::InputFloat(tablelabel.c_str(), &mat.m[row][col], 0, 0, "%.3f", ImGuiInputTextFlags_ReadOnly);
					}
				}
				ImGui::EndTable();
			}
		}
		break;
		}
	}

	// 전달된 텍스쳐 데이터
	vector<tTexData*> vecTexData = pCS->GetTexData();
	for (tTexData* data : vecTexData)
	{
		auto pTexPair = pCS->GetCSTex(static_cast<TEX_PARAM>(data->m_Param));
		if (pTexPair.first.Get())
		{
			ImGui::Text(data->m_Desc.c_str());
			string label = "##TextureData_" + to_string(data->m_Param);
			string name = to_str(pTexPair.first->GetName());
			ImGui::InputText(label.c_str(), const_cast<char*>(name.c_str()), name.size() + 1, ImGuiInputTextFlags_ReadOnly);
			if (ImGui::BeginItemTooltip())
			{
				ImTextureID TexID = (ImTextureID)pTexPair.first->GetSRV().Get();
				ImGui::Image(TexID, ImVec2(300, 300), ImVec2(0, 0), ImVec2(1, 1), ImVec4(1, 1, 1, 1), ImVec4(1, 1, 1, 1));
				ImGui::EndTooltip();
			}
		}
	}
    
    // 그룹 당 스레드 수
	float ThreadPerGroup[3] = { pCS->GetThreadPerGroup().x, pCS->GetThreadPerGroup().y, pCS->GetThreadPerGroup().z };
	ImGui::InputFloat3("##ThreadPerGroup", ThreadPerGroup, "%.3f", ImGuiInputTextFlags_ReadOnly);
	
    // 스레드 그룹 수
	float GroupCount[3] = { pCS->GetGroupCount().x, pCS->GetGroupCount().y, pCS->GetGroupCount().z };
	ImGui::InputFloat3("##GroupCount", GroupCount, "%.3f", ImGuiInputTextFlags_ReadOnly);

	// 컴퓨트 셰이더에서 추가적으로 렌더링해야 할 정보
	pCS->UIRender();
}
