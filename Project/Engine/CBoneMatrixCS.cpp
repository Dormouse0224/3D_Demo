#include "pch.h"
#include "CBoneMatrixCS.h"

#include "CStructuredBuffer.h"

CBoneMatrixCS::CBoneMatrixCS()
	: CComputeShader(L"HLSL\\Engine\\bonemat.fx", "CS_Animation3D", 256, 1, 1)
	, m_FrameDataBuffer(nullptr)
	, m_OffsetMatBuffer(nullptr)
	, m_OutputBuffer(nullptr)
{
}

CBoneMatrixCS::~CBoneMatrixCS()
{
}

int CBoneMatrixCS::Binding()
{
	// 구조화버퍼 전달
	m_FrameDataBuffer->Binding_CS(16); // t16
	m_OffsetMatBuffer->Binding_CS(17); // t17
	m_OutputBuffer->Binding_CS_UAV(0);   // u0

	return S_OK;
}

void CBoneMatrixCS::CalcGroupNum()
{
	m_GroupX = (m_Const.iArr[0] / m_ThreadPerGroupX) + 1;
	m_GroupY = 1;
	m_GroupZ = 1;
}

void CBoneMatrixCS::Clear()
{
	m_FrameDataBuffer->Clear_CS();
	m_OffsetMatBuffer->Clear_CS();
	m_OutputBuffer->Clear_CS_UAV();

	m_FrameDataBuffer = nullptr;
	m_OffsetMatBuffer = nullptr;
	m_OutputBuffer = nullptr;
}