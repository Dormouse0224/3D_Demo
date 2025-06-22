#include "pch.h"
#include "CMaterialUI.h"

#include "CGraphicShader.h"
#include "CTexture2D.h"
#include "CMaterial.h"
#include "CGraphicShader.h"

CMaterialUI::CMaterialUI()
	: CAssetUI(ASSET_TYPE_WSTR[(UINT)ASSET_TYPE::MATERIAL])
{
}

CMaterialUI::~CMaterialUI()
{
}

void CMaterialUI::Update_Ast()
{
}

void CMaterialUI::Render_Ast()
{
	float tab = 130;
	AssetPtr<CMaterial> pMaterial = static_cast<CMaterial*>(m_TargetAsset.Get());
	AssetPtr<CGraphicShader> pGS = pMaterial->GetShader();

	// 사용하는 그래픽 셰이더
	ImGui::Text("Graphic Shader");
	ImGui::SameLine(tab);
	string str = "No GraphicShader Exist";
	if (pGS.Get())
		str = to_str(pGS->GetName());
	ImGui::InputText("##GraphicShader", const_cast<char*>(str.c_str()), str.size() + 1, ImGuiInputTextFlags_ReadOnly);
	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ContentViewer"))
		{
			CAsset* Asset = *reinterpret_cast<CAsset**>(payload->Data);
			AssetPtr<CGraphicShader> pGS = dynamic_cast<CGraphicShader*>(Asset);
			if (pGS.Get())
			{
				pMaterial->SetShader(pGS);
			}
		}
		ImGui::EndDragDropTarget();
	}
	
	if (pGS.Get())
	{
		// 전달된 텍스쳐 데이터
		vector<tTexData*> vecTexData = pGS->GetTexData();
		for (tTexData* data : vecTexData)
		{
			ImGui::Text(data->m_Desc.c_str());
			string label = "##TextureData_" + to_string(data->m_Param);
			AssetPtr<CTexture2D> pTex = pMaterial->GetTexParam(static_cast<TEX_PARAM>(data->m_Param));
			string name = "No Texture2D Exist";
			if (pTex.Get())
				name = to_str(pMaterial->GetTexParam(static_cast<TEX_PARAM>(data->m_Param))->GetName());
			ImGui::InputText(label.c_str(), const_cast<char*>(name.c_str()), name.size() + 1, ImGuiInputTextFlags_ReadOnly);
			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ContentViewer"))
				{
					CAsset* Asset = *reinterpret_cast<CAsset**>(payload->Data);
					AssetPtr<CTexture2D> pTex = dynamic_cast<CTexture2D*>(Asset);
					if (pTex.Get())
					{
						pMaterial->SetTexParam(data->m_Param, pTex);
					}
				}
				ImGui::EndDragDropTarget();
			}
			if (ImGui::BeginItemTooltip())
			{
				if (pTex.Get())
				{
					ImTextureID TexID = (ImTextureID)pTex->GetSRV().Get();
					ImGui::Image(TexID, ImVec2(300, 300), ImVec2(0, 0), ImVec2(1, 1), ImVec4(1, 1, 1, 1), ImVec4(1, 1, 1, 1));
				}
				ImGui::EndTooltip();
			}
		}

		// 전달된 상수 데이터
		vector<tConstData*> vecConstData = pGS->GetConstData();
		tMtrlConst Const = pMaterial->GetConstParam();
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
				if (ImGui::InputInt(label.c_str(), &Const.iArr[data->m_Param - INT_0]))
				{
					pMaterial->SetScalarParam(data->m_Param, Const.iArr[data->m_Param - INT_0]);
				}
			}
			break;
			case FLOAT_0:
			case FLOAT_1:
			case FLOAT_2:
			case FLOAT_3:
			{
				if (ImGui::DragFloat(label.c_str(), &Const.fArr[data->m_Param - FLOAT_0], 0.001f))
				{
					pMaterial->SetScalarParam(data->m_Param, Const.fArr[data->m_Param - FLOAT_0]);
				}
			}
			break;
			case VEC2_0:
			case VEC2_1:
			case VEC2_2:
			case VEC2_3:
			{
				float vec[2] = { Const.v2Arr[data->m_Param - VEC2_0].x, Const.v2Arr[data->m_Param - VEC2_0].y };
				if (ImGui::DragFloat2(label.c_str(), vec, 0.001f))
				{
					pMaterial->SetScalarParam(data->m_Param, Vec2(vec));
				}
			}
			break;
			case VEC4_0:
			case VEC4_1:
			case VEC4_2:
			case VEC4_3:
			{
				Vec4 v4 = Const.v4Arr[data->m_Param - VEC4_0];
				float vec[4] = { v4.x, v4.y, v4.z, v4.w };
				if (ImGui::DragFloat4(label.c_str(), vec, 0.001f))
				{
					pMaterial->SetScalarParam(data->m_Param, Vec2(vec));
				}
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
	}
}
