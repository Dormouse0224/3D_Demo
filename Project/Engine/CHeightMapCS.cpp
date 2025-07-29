#include "pch.h"
#include "CHeightMapCS.h"

#include "CStructuredBuffer.h"

CHeightMapCS::CHeightMapCS()
    : CComputeShader(L"HLSL\\Engine\\heightmap.fx", "CS_HeightMap", 32, 32, 1)
    , m_RaycastOut(nullptr)
{
}

CHeightMapCS::~CHeightMapCS()
{
}

int CHeightMapCS::Binding()
{
    if (nullptr == m_HeightMap)
        return E_FAIL;

    m_HeightMap->Binding_CS_UAV(0);
    m_RaycastOut->Binding_CS(20);

    m_Const.iArr[0] = m_HeightMap->GetWidth();
    m_Const.iArr[1] = m_HeightMap->GetHeight();

    m_Const.v2Arr[0] = m_BrushScale;

    if (nullptr != m_BrushTex)
    {
        m_BrushTex->Binding_CS(0);
        m_Const.bTex[0] = 1;
    }
    else
        m_Const.bTex[0] = 0;

    return S_OK;
}

void CHeightMapCS::CalcGroupNum()
{
    m_GroupX = (m_HeightMap->GetWidth() / m_ThreadPerGroupX) + !!(m_HeightMap->GetWidth() % m_ThreadPerGroupX);
    m_GroupY = (m_HeightMap->GetHeight() / m_ThreadPerGroupY) + !!(m_HeightMap->GetHeight() % m_ThreadPerGroupY);
    m_GroupZ = 1;
}

void CHeightMapCS::Clear()
{
    m_HeightMap->Unbind_CS_UAV();
    m_RaycastOut->Unbind_CS();
}

void CHeightMapCS::UIRender()
{
}

void CHeightMapCS::SetDataInfo()
{
}
