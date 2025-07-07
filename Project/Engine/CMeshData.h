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
    // FBX 파일로부터 Mesh 와 Material 정보들을 읽어옵니다.
    static CMeshData* LoadFromFBX(const wstring& _RelativePath);

    // MeshData 를 기반으로 오브젝트를 생성합니다.
    CGameObject* Instantiate();

};



