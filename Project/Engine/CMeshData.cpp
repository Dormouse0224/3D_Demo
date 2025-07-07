#include "pch.h"
#include "CMeshData.h"

#include "CPathMgr.h"
#include "CAssetMgr.h"

#include "CGameObject.h"
#include "CTransform.h"
#include "CMeshRender.h"
#include "CAnimator3D.h"

#include "CFBXLoader.h"

CMeshData::CMeshData()
	: CAsset(ASSET_TYPE::MESHDATA)
{
}

CMeshData::~CMeshData()
{
}

CGameObject* CMeshData::Instantiate()
{
	CGameObject* pNewObj = new CGameObject;
	pNewObj->AddComponent(new CTransform);
	pNewObj->AddComponent(new CMeshRender);

	pNewObj->MeshRender()->SetMesh(m_pMesh);

	for (UINT i = 0; i < m_vecMtrl.size(); ++i)
	{
		pNewObj->MeshRender()->SetMaterial(m_vecMtrl[i], i);
	}

    // Animation 파트 추가
    if (false == m_pMesh->IsAnimMesh())
        return pNewObj;

    CAnimator3D* pAnimator = new CAnimator3D;
    pNewObj->AddComponent(pAnimator);

    pAnimator->SetBones(m_pMesh->GetBones());
    pAnimator->SetAnimClip(m_pMesh->GetAnimClip());
    
	return pNewObj;
}

CMeshData* CMeshData::LoadFromFBX(const wstring& _RelativePath)
{
	wstring strFullPath = CPathMgr::GetContentDir();
	strFullPath += _RelativePath;

    CFBXLoader loader;
    loader.init();
    loader.LoadFbx(strFullPath);

	// 메쉬 가져오기
	CMesh* pMesh = nullptr;
	pMesh = CMesh::CreateFromContainer(loader);

	// AssetMgr 에 메쉬 등록
	if (nullptr != pMesh)
	{
		wstring strMeshKey = L"Mesh\\";
        wstring MeshName = path(strFullPath).stem();
        strMeshKey += MeshName;
		strMeshKey += L".mesh";
		CAssetMgr::GetInst()->AddAsset(strMeshKey, pMesh);

		// 메시를 실제 파일로 저장
		pMesh->Save(MeshName);
	}

	vector<AssetPtr<CMaterial>> vecMtrl;

	// 메테리얼 가져오기
	for (UINT i = 0; i < loader.GetContainer(0).vecMtrl.size(); ++i)
	{
		// 예외처리 (material 이름이 입력 안되어있을 수도 있다.)
        AssetPtr<CMaterial> pMtrl = CAssetMgr::GetInst()->Load<CMaterial>(loader.GetContainer(0).vecMtrl[i].strMtrlName, true);
		assert(pMtrl.Get());

		vecMtrl.push_back(pMtrl);
	}

    CMeshData* pMeshData = new CMeshData;
	pMeshData->m_pMesh = pMesh;
	pMeshData->m_vecMtrl = vecMtrl;

    return pMeshData;
}


int CMeshData::Save(const wstring& _FileName, bool _Update)
{
    // MeshData 경로 생성 후 출력 모드로 파일 스트림 오픈
    std::filesystem::path path = CPathMgr::GetContentDir() + std::wstring(L"MeshData\\") + _FileName + std::wstring(L".mdat");
    CPathMgr::CreateParentDir(path);
    std::fstream file(path, std::ios::out | std::ios::binary);
    if (!file.is_open())
        return E_FAIL;
    
    // MeshData 에서 사용하는 Mesh 의 키값을 저장
    wstring meshKey = m_pMesh->GetKey();
    int len = meshKey.length();
    file.write(reinterpret_cast<char*>(&len), sizeof(int));
    if (len > 0)
        file.write(reinterpret_cast<char*>(meshKey.data()), sizeof(wchar_t) * len);

    // MeshData 에서 사용하는 Material 의 키값을 저장
    int count = m_vecMtrl.size();
    file.write(reinterpret_cast<char*>(&count), sizeof(int));
    for (int i = 0; i < count; ++i)
    {
        wstring mtrlKey = m_vecMtrl[i]->GetKey();
        int len = mtrlKey.length();
        file.write(reinterpret_cast<char*>(&len), sizeof(int));
        if (len > 0)
            file.write(reinterpret_cast<char*>(mtrlKey.data()), sizeof(wchar_t) * len);
    }

    file.close();

	return S_OK;
}

int CMeshData::Load(const wstring& _FilePath)
{
    // 입력 모드로 파일 스트림 오픈
    std::fstream file(CPathMgr::GetContentDir() + _FilePath, std::ios::in | std::ios::binary);

    if (!file.is_open())
    {
        MessageBoxW(nullptr, L"파일을 찾을 수 없습니다.\n경로를 확인해 주세요.", L"MeshData Load Error", MB_OK);
        return E_FAIL;
    }


    // MeshData 에서 사용하는 Mesh 의 키값을 불러오기
    wstring meshKey = {};
    int len = 0;
    file.read(reinterpret_cast<char*>(&len), sizeof(int));
    if (len > 0)
    {
        meshKey.resize(len);
        file.read(reinterpret_cast<char*>(meshKey.data()), sizeof(wchar_t) * len);
        m_pMesh = CAssetMgr::GetInst()->Load<CMesh>(meshKey);
    }

    // MeshData 에서 사용하는 Material 의 키값을 불러오기
    int count = 0;
    file.read(reinterpret_cast<char*>(&count), sizeof(int));
    for (int i = 0; i < count; ++i)
    {
        wstring meshKey = {};
        int len = 0;
        file.read(reinterpret_cast<char*>(&len), sizeof(int));
        if (len > 0)
        {
            meshKey.resize(len);
            file.read(reinterpret_cast<char*>(meshKey.data()), sizeof(wchar_t) * len);
            m_vecMtrl.push_back(CAssetMgr::GetInst()->Load<CMaterial>(meshKey));
        }
    }

    file.close();

	return S_OK;
}