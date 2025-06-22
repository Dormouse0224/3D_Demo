#include "pch.h"
#include "CEntity.h"

UINT CEntity::g_NextID = 0;

CEntity::CEntity()
	: m_ID(g_NextID++)
{
}

CEntity::CEntity(wstring _Name)
	: m_Name(_Name)
	, m_ID(g_NextID++)
{
}

CEntity::CEntity(const CEntity& _Origin)
	: m_ID(g_NextID++)
	, m_Name(_Origin.m_Name)
{
}

CEntity::~CEntity()
{
}

void CEntity::SaveName(fstream& _Stream)
{
	std::wstring Name = GetName();
	int size = Name.size();
	_Stream.write(reinterpret_cast<char*>(&size), sizeof(int));
	_Stream.write(reinterpret_cast<char*>(Name.data()), sizeof(wchar_t) * size);
}

void CEntity::LoadName(fstream& _Stream)
{
	std::wstring Name = {};
	int size = Name.size();
	_Stream.read(reinterpret_cast<char*>(&size), sizeof(int));
	Name.resize(size);
	_Stream.read(reinterpret_cast<char*>(Name.data()), sizeof(wchar_t) * size);
	SetName(Name);
}
