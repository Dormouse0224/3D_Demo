#include "pch.h"
#include "CShader.h"

CShader::CShader(ASSET_TYPE _Type)
	: CAsset(_Type)
{
}

CShader::~CShader()
{
	for (int i = 0; i < m_vecConstData.size(); ++i)
	{
		delete m_vecConstData[i];
	}
	for (int i = 0; i < m_vecTexData.size(); ++i)
	{
		delete m_vecTexData[i];
	}
}
