#pragma once


class CEntity
{
public:
	CEntity();
	CEntity(wstring _Name);
	CEntity(const CEntity& _Origin);
	virtual ~CEntity();
	virtual CEntity* Clone() = 0;	// 자기자신의 복제객체를 반환해주는 함수

private:
	static UINT g_NextID;
	UINT		m_ID;
	wstring		m_Name;

public:
	void SetName(wstring _Name) { m_Name = _Name; }
	const wstring& GetName() { return m_Name; }

	UINT GetID() { return m_ID; }

	void SaveName(fstream& _Stream);
	void LoadName(fstream& _Stream);

};


typedef void(CEntity::* DELEGATE_0)();
typedef void(CEntity::* DELEGATE_1)(DWORD_PTR);
typedef void(CEntity::* DELEGATE_2)(DWORD_PTR, DWORD_PTR);

