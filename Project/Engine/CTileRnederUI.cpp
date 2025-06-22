#include "pch.h"
#include "CTileRnederUI.h"
#include "CTileRender.h"

#include "CKeyMgr.h"



CTileRnederUI::CTileRnederUI()
	: CRenderComponentUI(COMPONENT_TYPE::TILERENDER)
    , m_TIlemapEditorOpen(false)
{
}

CTileRnederUI::~CTileRnederUI()
{
}

void CTileRnederUI::Update_Com()
{
}

void CTileRnederUI::Render_Com()
{
    Render_RCom();

    UINT Col = static_cast<CTileRender*>(m_TargetObj->GetRenderComponent())->m_Col;
    UINT Row = static_cast<CTileRender*>(m_TargetObj->GetRenderComponent())->m_Row;
    Vec2 TileSize = static_cast<CTileRender*>(m_TargetObj->GetRenderComponent())->m_TileSize;
    AssetPtr<CTexture2D> AtlasTex = static_cast<CTileRender*>(m_TargetObj->GetRenderComponent())->m_AtlasTex;
    vector<TileInfo>& vecTIleInfo = static_cast<CTileRender*>(m_TargetObj->GetRenderComponent())->m_vecTileInfo;

    ImGui::SeparatorText("Tile Render Option");
    ImGui::Text("Column");
    ImGui::SameLine();
    if (ImGui::InputScalar("##Col", ImGuiDataType_U32, &Col))
    {
        m_TargetObj->TileRender()->SetColRow(Col, Row);
    }

    ImGui::Text("Row");
    ImGui::SameLine();
    if (ImGui::InputScalar("##Row", ImGuiDataType_U32, &Row))
    {
        m_TargetObj->TileRender()->SetColRow(Col, Row);
    }

    ImGui::Text("Tile Size");
    ImGui::SameLine();
    float farrTileSize[] = { TileSize.x, TileSize.y };
    if (ImGui::InputFloat2("##TileSize", farrTileSize))
    {
        m_TargetObj->TileRender()->SetTileSize(Vec2(farrTileSize[0], farrTileSize[1]));
    }

    ImGui::Text("Atlas Texture");
    std::string AtlasTexName = "NO EXIST";
    if (AtlasTex.Get())
    {
        AtlasTexName = to_str(AtlasTex->GetName());
    }
    ImGui::InputText("##AtlasTex", const_cast<char*>(AtlasTexName.c_str())
        , AtlasTexName.size() + 1, ImGuiInputTextFlags_ReadOnly);
    if (ImGui::BeginDragDropTarget())
    {
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ContentViewer"))
        {
            CAsset* Asset = *reinterpret_cast<CAsset**>(payload->Data);
            AssetPtr<CTexture2D> pTex = dynamic_cast<CTexture2D*>(Asset);
            if (pTex.Get())
            {
                m_TargetObj->TileRender()->SetAtlasTex(pTex);
            }
        }
        ImGui::EndDragDropTarget();
    }

    if (ImGui::Button("Open Tilemap Editor"))
    {
        m_TIlemapEditorOpen = true;
    }
    TIlemapEditor(Col, Row, TileSize, AtlasTex, vecTIleInfo);

}

void CTileRnederUI::TIlemapEditor(UINT& _Col, UINT& _Row, Vec2& _TileSize, AssetPtr<CTexture2D> _AtlasTex, vector<TileInfo>& _vecTIleInfo)
{
    static Vec2 originOffset(0, 0);
    static bool bEraseMode = false;
    static Vec2 selectedSprite(-1, -1);
    
    // 창 종료 시 세팅 초기화
    if (!m_TIlemapEditorOpen)
    {
        originOffset = Vec2(0, 0);
        bEraseMode = false;
        selectedSprite = Vec2(-1, -1);
    }
    else
    {
        // 창 좌측 : 타일맵 격자 뷰어, 편집기 도구
        // 1. 타일맵 격자 뷰어
        ImGui::Begin("TIlemap Editor", &m_TIlemapEditorOpen);
        ImGui::BeginGroup();
        {
            ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 5.0f);
            ImGui::BeginChild("Tilemap", ImVec2(500, 500), ImGuiChildFlags_Borders);
            {
                ImDrawList* draw_list = ImGui::GetWindowDrawList();
                Vec2 ViewerPos(ImGui::GetCursorScreenPos().x, ImGui::GetCursorScreenPos().y);
                Vec2 origin = ViewerPos; // 그리기 시작 위치

                // 마우스 우클릭 키다운으로 드래그
                if (ImGui::IsWindowHovered() && CKeyMgr::GetInst()->GetKeyState(Keyboard::MOUSE_RBTN) == Key_state::PRESSED)
                {
                    originOffset += CKeyMgr::GetInst()->GetDragDir();
                }
                origin += originOffset;

                // 뷰포트 사이즈
                ImVec2 canvas_size = ImGui::GetContentRegionAvail();

                // 마우스가 가리키는 타일 체크
                Vec2 MouseHoverTile(-1, -1);
                if (ImGui::IsWindowFocused())
                {
                    MouseHoverTile = Vec2(static_cast<UINT>((ImGui::GetMousePos().x - origin.x) / _TileSize.x)
                        , static_cast<UINT>((ImGui::GetMousePos().y - origin.y) / _TileSize.y));
                    
                }

                draw_list->PushClipRect(ImVec2(ViewerPos.x, ViewerPos.y), ImVec2(ViewerPos.x + canvas_size.x, ViewerPos.y + canvas_size.y), true);
                for (UINT y = 0; y < _Row; ++y)
                {
                    for (UINT x = 0; x < _Col; ++x)
                    {
                        ImVec2 pos_min = ImVec2(origin.x + x * _TileSize.x,
                            origin.y + y * _TileSize.y);
                        ImVec2 pos_max = ImVec2(pos_min.x + _TileSize.x, pos_min.y + _TileSize.y);

                        // 기본 빈 타일 : 회색 테두리, 내부색 없음
                        // 채워진 타일 : 초록색으로 내부 채움
                        // 마우스 커서가 위치한 타일 : 테두리를 붉은색으로 강조 표시
                        ImU32 edgeCol, fillCol;
                        MouseHoverTile == Vec2(x, y) ? edgeCol = IM_COL32(255, 100, 100, 255) : edgeCol = IM_COL32(100, 100, 100, 255);
                        _vecTIleInfo[_Col * y + x].vLeftTop == Vec2(-1, -1) ? fillCol = IM_COL32(0, 0, 0, 0) : fillCol = IM_COL32(100, 255, 100, 255);
                        draw_list->AddRectFilled(pos_min, pos_max, fillCol);
                        draw_list->AddRect(pos_min, pos_max, edgeCol);
                    }
                }
                draw_list->PopClipRect();

                // 클릭 발생 시 마우스가 가리키는 타일을 편집
                if (ImGui::IsWindowHovered() && CKeyMgr::GetInst()->GetKeyState(Keyboard::MOUSE_LBTN) == Key_state::PRESSED)
                {
                    if (MouseHoverTile.x >= 0 && MouseHoverTile.x < _Col && MouseHoverTile.y >= 0 && MouseHoverTile.y < _Row)
                    {
                        if (!bEraseMode)
                        {
                            if (selectedSprite != Vec2(-1, -1))
                                m_TargetObj->TileRender()->SetTile(MouseHoverTile.x, MouseHoverTile.y, selectedSprite);
                        }
                        else
                        {
                            m_TargetObj->TileRender()->SetTile(MouseHoverTile.x, MouseHoverTile.y, Vec2(-1, -1));
                        }
                    }
                }
            }
            ImGui::EndChild();
            ImGui::PopStyleVar();

            // 2. 편집기 도구
            ImGui::NewLine();
            ImGui::Checkbox("Erase Mode", &bEraseMode);
        }
        ImGui::EndGroup();

        // 창 우측 : 아틀라스 이미지 분할 뷰어, 스프라이트 뷰어
        ImGui::SameLine();
        ImGui::BeginGroup();
        {
            // 아틀라스 텍스쳐
            ImGui::Text("Atlas Texture");
            if (_AtlasTex.Get())
            {
                // 아틀라스 이미지 출력
                float ImageWidth = 320.f;
                Vec2 origin(ImGui::GetCursorScreenPos().x, ImGui::GetCursorScreenPos().y);
                ImGui::Image(reinterpret_cast<ImTextureID>(_AtlasTex->GetSRV().Get())
                    , ImVec2(ImageWidth, ImageWidth * (static_cast<float>(_AtlasTex->GetHeight()) / static_cast<float>(_AtlasTex->GetWidth())))
                    , ImVec2(0, 0), ImVec2(1, 1), ImVec4(1, 1, 1, 1), ImVec4(1, 1, 1, 1));
                float scale = ImageWidth / static_cast<float>(_AtlasTex->GetWidth());

                // 드로우 리스트, 아틀라스 이미지의 위치, 아틀라스 분할 개수
                ImDrawList* draw_list = ImGui::GetWindowDrawList();
                Vec2 SliceCount(0, 0);
                if (_Col != 0 && _Row != 0)
                    SliceCount = Vec2(_AtlasTex->GetWidth() / _TileSize.x, _AtlasTex->GetHeight() / _TileSize.y);

                // 마우스가 가리키는 타일 체크
                Vec2 MouseHoverTile(-1, -1);
                if (ImGui::IsWindowFocused())
                    MouseHoverTile = Vec2(static_cast<UINT>((ImGui::GetMousePos().x - origin.x) / (_TileSize.x * scale))
                        , static_cast<UINT>((ImGui::GetMousePos().y - origin.y) / (_TileSize.y * scale)));

                // 격자 그리기
                for (int row = 0; row < SliceCount.y; ++row)
                {
                    for (int col = 0; col < SliceCount.x; ++col)
                    {
                        ImVec2 pos_min(col * _TileSize.x * scale + origin.x, row * _TileSize.y * scale + origin.y);
                        ImVec2 pos_max(pos_min.x + (_TileSize.x * scale), pos_min.y + (_TileSize.y * scale));
                        ImU32 edgeCol;
                        MouseHoverTile == Vec2(col, row) ? edgeCol = IM_COL32(255, 100, 100, 255) : edgeCol = IM_COL32(100, 100, 100, 255);
                        draw_list->AddRect(pos_min, pos_max, edgeCol);
                    }
                }

                // 클릭 발생 시 마우스가 가리키는 타일을 스프라이트 대상으로 선택
                if (ImGui::IsItemHovered() && CKeyMgr::GetInst()->GetKeyState(Keyboard::MOUSE_LBTN) == Key_state::TAP)
                {
                    if (MouseHoverTile.x >= 0 && MouseHoverTile.x < SliceCount.x && MouseHoverTile.y >= 0 && MouseHoverTile.y < SliceCount.y)
                        selectedSprite = Vec2(_TileSize.x * MouseHoverTile.x, _TileSize.y * MouseHoverTile.y);
                }
            }
            else
            {
                ImGui::TextColored(ImVec4(1.f, 0.2f, 0.2f, 1.f), "N/A");
            }

            // 선택된 스프라이트
            ImGui::NewLine();
            ImGui::Text("Selected Sprite");
            if (selectedSprite != Vec2(-1, -1))
            {
                float ImageWidth = 160.f;
                ImVec2 uv_min(selectedSprite.x / _AtlasTex->GetWidth(), selectedSprite.y / _AtlasTex->GetHeight());
                ImVec2 uv_max((selectedSprite.x + _TileSize.x) / _AtlasTex->GetWidth(), (selectedSprite.y + _TileSize.y) / _AtlasTex->GetHeight());
                ImGui::Image(reinterpret_cast<ImTextureID>(_AtlasTex->GetSRV().Get())
                    , ImVec2(ImageWidth, ImageWidth * (_TileSize.y / _TileSize.x))
                    , uv_min, uv_max, ImVec4(1, 1, 1, 1), ImVec4(1, 1, 1, 1));
            }
            else
            {
                ImGui::TextColored(ImVec4(1.f, 0.2f, 0.2f, 1.f), "N/A");
            }
        }
        ImGui::EndGroup();

        ImGui::End();
    }

}
