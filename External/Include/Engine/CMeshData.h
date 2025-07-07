#pragma once
#include "CAsset.h"

#include "CMesh.h"
#include "CMaterial.h"

class CMeshData :
	public CAsset
{
public:
    CMeshData();
    ~CMeshData();
    CLONE_DISABLE(CMeshData);

private:
	AssetPtr<CMesh>				m_pMesh;
	vector<AssetPtr<CMaterial>>	m_vecMtrl;

public:
	virtual int Save(const wstring& _FileName, bool _Update = false) override;
	virtual int Load(const wstring& _FilePath) override;

public:
    // FBX ���Ϸκ��� Mesh �� Material �������� �о�ɴϴ�.
    static CMeshData* LoadFromFBX(const wstring& _RelativePath);

    // MeshData �� ������� ������Ʈ�� �����մϴ�.
    CGameObject* Instantiate();

};



