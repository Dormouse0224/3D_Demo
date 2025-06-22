#include "pch.h"
#include "CAsset.h"

CAsset::CAsset(ASSET_TYPE _Type)
	: CEntity()
	, m_Type(_Type)
	, m_RefCount(0)
	, m_Key{}
	, m_RelativePath{}
	, m_EngineAsset(false)
{
}

CAsset::CAsset(const CAsset& _Other)
	: CEntity(_Other)
	, m_Type(_Other.m_Type)
	, m_RefCount(0)
	, m_Key{}
	, m_RelativePath{}
	, m_EngineAsset(true)
{
}

CAsset::~CAsset()
{
}
