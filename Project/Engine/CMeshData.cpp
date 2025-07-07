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

    // Animation ��Ʈ �߰�
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

	// �޽� ��������
	CMesh* pMesh = nullptr;
	pMesh = CMesh::CreateFromContainer(loader);

	// AssetMgr �� �޽� ���
	if (nullptr != pMesh)
	{
		wstring strMeshKey = L"Mesh\\";
        wstring MeshName = path(strFullPath).stem();
        strMeshKey += MeshName;
		strMeshKey += L".mesh";
		CAssetMgr::GetInst()->AddAsset(strMeshKey, pMesh);

		// �޽ø� ���� ���Ϸ� ����
		pMesh->Save(MeshName);
	}

	vector<AssetPtr<CMaterial>> vecMtrl;

	// ���׸��� ��������
	for (UINT i = 0; i < loader.GetContainer(0).vecMtrl.size(); ++i)
	{
		// ����ó�� (material �̸��� �Է� �ȵǾ����� ���� �ִ�.)
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
    // MeshData ��� ���� �� ��� ���� ���� ��Ʈ�� ����
    std::filesystem::path path = CPathMgr::GetContentDir() + std::wstring(L"MeshData\\") + _FileName + std::wstring(L".mdat");
    CPathMgr::CreateParentDir(path);
    std::fstream file(path, std::ios::out | std::ios::binary);
    if (!file.is_open())
        return E_FAIL;
    
    // MeshData ���� ����ϴ� Mesh �� Ű���� ����
    wstring meshKey = m_pMesh->GetKey();
    int len = meshKey.length();
    file.write(reinterpret_cast<char*>(&len), sizeof(int));
    if (len > 0)
        file.write(reinterpret_cast<char*>(meshKey.data()), sizeof(wchar_t) * len);

    // MeshData ���� ����ϴ� Material �� Ű���� ����
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
    // �Է� ���� ���� ��Ʈ�� ����
    std::fstream file(CPathMgr::GetContentDir() + _FilePath, std::ios::in | std::ios::binary);

    if (!file.is_open())
    {
        MessageBoxW(nullptr, L"������ ã�� �� �����ϴ�.\n��θ� Ȯ���� �ּ���.", L"MeshData Load Error", MB_OK);
        return E_FAIL;
    }


    // MeshData ���� ����ϴ� Mesh �� Ű���� �ҷ�����
    wstring meshKey = {};
    int len = 0;
    file.read(reinterpret_cast<char*>(&len), sizeof(int));
    if (len > 0)
    {
        meshKey.resize(len);
        file.read(reinterpret_cast<char*>(meshKey.data()), sizeof(wchar_t) * len);
        m_pMesh = CAssetMgr::GetInst()->Load<CMesh>(meshKey);
    }

    // MeshData ���� ����ϴ� Material �� Ű���� �ҷ�����
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