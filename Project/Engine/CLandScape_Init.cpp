#include "pch.h"
#include "CLandScape.h"

#include "CAssetMgr.h"
#include "CTexture.h"

void CLandScape::CreateLandScapeMesh()
{
	vector<Vtx> vecVtx;
	vector<UINT> vecIdx;
	Vtx v;

	// Vtx
	for (UINT Row = 0; Row < m_FaceZ + 1; ++Row)
	{
		for (UINT Col = 0; Col < m_FaceX + 1; ++Col)
		{
			v.vPos = Vec3(Col, 0.f, Row);
			v.vUV = Vec2(Col, m_FaceZ - Row);
			v.vNormal = Vec3(0.f, 1.f, 0.f);
			v.vTangent = Vec3(1.f, 0.f, 0.f);
			v.vBinormal = Vec3(0.f, 0.f, -1.f);
			v.vColor = Vec4(1.f, 1.f, 1.f, 1.f);

			vecVtx.push_back(v);
		}
	}

	// Index
	for (UINT Row = 0; Row < m_FaceZ; ++Row)
	{
		for (UINT Col = 0; Col < m_FaceX; ++Col)
		{
			// 0 
			// | \
			// 2--1		
			vecIdx.push_back((m_FaceX + 1) * (Row + 1) + (Col));
			vecIdx.push_back((m_FaceX + 1) * (Row)     + (Col + 1));
			vecIdx.push_back((m_FaceX + 1) * (Row)     + (Col));

			// 1--2 
			//  \ |
			//    0		
			vecIdx.push_back((m_FaceX + 1) * (Row)     + (Col + 1));
			vecIdx.push_back((m_FaceX + 1) * (Row + 1) + (Col));
			vecIdx.push_back((m_FaceX + 1) * (Row + 1) + (Col + 1));
		}
	}

	AssetPtr<CMesh> pMesh = new CMesh;
	pMesh->Create(vecVtx.data(), (UINT)vecVtx.size(), vecIdx.data(), vecIdx.size());
    pMesh->SetName(L"Landscape Mesh");
	SetMesh(pMesh);
}

void CLandScape::CreateComputeShader()
{
	m_HeightMapCS = new CHeightMapCS;

	m_RaycastCS = new CRaycastCS;

	m_WeightMapCS = new CWeightMapCS;
}

void CLandScape::CreateWeightMap()
{
    // 가중치 WeightMap 용 StructuredBuffer
    if (m_WeightMap)
        delete m_WeightMap;
    m_WeightMap = new CStructuredBuffer;
    m_WeightMap->Create(sizeof(tWeight8), m_WeightWidth * m_WeightHeight, SB_TYPE::SRV_UAV, true, nullptr);
}

void CLandScape::CreateHeightMap(UINT _Width, UINT _Height)
{
    wstring key = L"HeightMap" + to_wstring(GetID());
	if (nullptr != m_HeightMap)
	{
		CAssetMgr::GetInst()->DeleteAsset(ASSET_TYPE::TEXTURE, key.c_str());
	}

	m_HeightMap = CAssetMgr::GetInst()->CreateTexture(key.c_str()
		, _Width, _Height, DXGI_FORMAT_R32_FLOAT
		, D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS);
}

void CLandScape::CreateHeightMap(AssetPtr<CTexture> _Tex)
{
    wstring key = L"HeightMap" + to_wstring(GetID());
    if (nullptr != m_HeightMap)
    {
        CAssetMgr::GetInst()->DeleteAsset(ASSET_TYPE::TEXTURE, key.c_str());
    }

    m_HeightMap = CAssetMgr::GetInst()->CreateTexture(key.c_str(), _Tex->GetTex());
}

void CLandScape::SetColorTexture(AssetPtr<CTexture> _ArrTex)
{
    AssetPtr<CTexture> pTex = _ArrTex;

	if (pTex->GetMetaData().mipLevels <= 1)
    {
        if (!FAILED(pTex->GenerateMip(5)))
            m_ColorTex = pTex;
    }
}

void CLandScape::SetNormalTexture(AssetPtr<CTexture> _ArrTex)
{
    AssetPtr<CTexture> pTex = _ArrTex;

    if (pTex->GetMetaData().mipLevels <= 1)
    {
        if (!FAILED(pTex->GenerateMip(5)))
            m_NormalTex = pTex;
    }
}
