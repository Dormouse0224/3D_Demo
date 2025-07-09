#include "pch.h"
#include "CAssetMgr.h"

#include "CPathMgr.h"
#include "assets.h"

void CAssetMgr::Init()
{
	CreateEngineMesh();

	CreateEngineGraphicShader();

	CreateEngineMaterial();

	m_DirNotifyHandle = FindFirstChangeNotification(CPathMgr::GetContentDir(), true
													, FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_DIR_NAME
													| FILE_NOTIFY_CHANGE_LAST_WRITE | FILE_NOTIFY_CHANGE_CREATION);
}

void CAssetMgr::CreateEngineMesh()
{
	// =========
	// PointMesh
	// =========
	AssetPtr<CMesh> pMesh = new CMesh;
	Vtx vPoint;
	vPoint.vPos = Vec3(0.f, 0.f, 0.f);
	vPoint.vUV = Vec2(0.f, 0.f);
	vPoint.vColor = Vec4(1.f, 1.f, 1.f, 1.f);
	UINT Idx = 0;
	pMesh->Create(&vPoint, 1, &Idx, 1);
	pMesh->SetEngineAsset(true);
	AddAsset(L"EA_PointMesh", pMesh.Get());

    // =========
    // LineMesh
    // =========
    pMesh = new CMesh;
    Vtx arrLine[2];
    arrLine[0].vPos = Vec3(0.f, 0.f, 0.f);
    arrLine[0].vColor = Vec4(1.f, 0.f, 1.f, 1.f);
    arrLine[0].vUV = Vec2(0.f, 0.f);

    arrLine[1].vPos = Vec3(1.f, 0.f, 0.f);
    arrLine[1].vColor = Vec4(1.f, 0.f, 1.f, 1.f);
    arrLine[1].vUV = Vec2(1.f, 0.f);

    // Index
    UINT arrLineIdx[2] = { 0, 1 };

    pMesh->Create(arrLine, 2, arrLineIdx, 2);
    pMesh->SetEngineAsset(true);
    AddAsset(L"EA_LineMesh", pMesh.Get());

	// ========
	// RectMesh
	// ========	
	// 0 -- 1
	// | \  |
	// 3 -- 2
	pMesh = new CMesh;

	Vtx arrVtx[4] = {};
	arrVtx[0].vPos = Vec3(-0.5f, 0.5f, 0.f);
	arrVtx[0].vColor = Vec4(1.f, 0.f, 1.f, 1.f);
	arrVtx[0].vUV = Vec2(0.f, 0.f);
	arrVtx[0].vTangent = Vec3(1.f, 0.f, 0.f);
	arrVtx[0].vBinormal = Vec3(0.f, -1.f, 0.f);
	arrVtx[0].vNormal = Vec3(0.f, 0.f, -1.f);

	arrVtx[1].vPos = Vec3(0.5f, 0.5f, 0.f);
	arrVtx[1].vColor = Vec4(1.f, 0.f, 1.f, 1.f);
	arrVtx[1].vUV = Vec2(1.f, 0.f);
	arrVtx[1].vTangent = Vec3(1.f, 0.f, 0.f);
	arrVtx[1].vBinormal = Vec3(0.f, -1.f, 0.f);
	arrVtx[1].vNormal = Vec3(0.f, 0.f, -1.f);

	arrVtx[2].vPos = Vec3(0.5f, -0.5f, 0.f);
	arrVtx[2].vColor = Vec4(1.f, 0.f, 1.f, 1.f);
	arrVtx[2].vUV = Vec2(1.f, 1.f);
	arrVtx[2].vTangent = Vec3(1.f, 0.f, 0.f);
	arrVtx[2].vBinormal = Vec3(0.f, -1.f, 0.f);
	arrVtx[2].vNormal = Vec3(0.f, 0.f, -1.f);

	arrVtx[3].vPos = Vec3(-0.5f, -0.5f, 0.f);
	arrVtx[3].vColor = Vec4(1.f, 0.f, 1.f, 1.f);
	arrVtx[3].vUV = Vec2(0.f, 1.f);
	arrVtx[3].vTangent = Vec3(1.f, 0.f, 0.f);
	arrVtx[3].vBinormal = Vec3(0.f, -1.f, 0.f);
	arrVtx[3].vNormal = Vec3(0.f, 0.f, -1.f);

	// Index
	UINT arrIdx[6] = { 0, 1, 2, 0, 2, 3 };

	pMesh->Create(arrVtx, 4, arrIdx, 6);
	pMesh->SetEngineAsset(true);
	AddAsset(L"EA_RectMesh", pMesh.Get());


	// ==============
	// RectMesh_Debug
	// ==============	
	// 0 -- 1
	// | \  |
	// 3 -- 2
	pMesh = new CMesh;

	arrIdx[0] = 0; arrIdx[1] = 1; arrIdx[2] = 2; arrIdx[3] = 3; arrIdx[4] = 0;
	pMesh->Create(arrVtx, 4, arrIdx, 5);
	pMesh->SetEngineAsset(true);
	AddAsset(L"EA_RectMesh_Debug", pMesh.Get());

	// ==========
	// CircleMesh
	// ==========
	vector<Vtx> vecVtx;
	vector<UINT> vecIdx;


	pMesh = new CMesh;

	// Circle 중심점
	Vtx v;
	v.vPos = Vec3(0.f, 0.f, 0.f);
	v.vColor = Vec4(1.f, 1.f, 1.f, 1.f);
	v.vUV = Vec2(0.5f, 0.5f);
	vecVtx.push_back(v);

	int Slice = 40;
	float Radius = 0.5f;
	float Angle = 0.f;

	// Circle 테두리 정점
	for (int i = 0; i < Slice + 1; ++i)
	{
		v.vPos = Vec3(Radius * cosf(Angle), Radius * sinf(Angle), 0.f);
		v.vUV = Vec2(v.vPos.x + 0.5f, 1.f - (v.vPos.y + 0.5f));
		vecVtx.push_back(v);

		Angle += XM_2PI / (float)Slice;
	}

	// Circle 인덱스
	for (int i = 0; i < Slice; ++i)
	{
		vecIdx.push_back(0);
		vecIdx.push_back(i + 2);
		vecIdx.push_back(i + 1);
	}

	pMesh->Create(vecVtx.data(), (UINT)vecVtx.size(), vecIdx.data(), (UINT)vecIdx.size());
	pMesh->SetEngineAsset(true);
	AddAsset(L"EA_CircleMesh", pMesh.Get());
	vecIdx.clear();

	// ================
	// CircleMesh_Debug
	// ================
	pMesh = new CMesh;

	for (size_t i = 0; i < vecVtx.size() - 1; ++i)
	{
		vecIdx.push_back(i + 1);
	}
	pMesh->Create(vecVtx.data(), (UINT)vecVtx.size(), vecIdx.data(), (UINT)vecIdx.size());
	pMesh->SetEngineAsset(true);
	AddAsset(L"EA_CircleMesh_Debug", pMesh.Get());


    // ========
    // CubeMesh
    // ========
    // 24개의 정점이 필요
    // 평면 하나당 정점 4개 x 6면 = 24개
    vecVtx.clear();
    vecIdx.clear();

    Vtx arrCube[24] = {};

    // 윗면
    arrCube[0].vPos = Vec3(-0.5f, 0.5f, 0.5f);
    arrCube[0].vColor = Vec4(1.f, 1.f, 1.f, 1.f);
    arrCube[0].vUV = Vec2(0.f, 0.f);
    arrCube[0].vTangent = Vec3(1.f, 0.f, 0.f);
    arrCube[0].vNormal = Vec3(0.f, 1.f, 0.f);
    arrCube[0].vBinormal = Vec3(0.f, 0.f, -1.f);

    arrCube[1].vPos = Vec3(0.5f, 0.5f, 0.5f);
    arrCube[1].vColor = Vec4(1.f, 1.f, 1.f, 1.f);
    arrCube[1].vUV = Vec2(1.f, 0.f);
    arrCube[1].vTangent = Vec3(1.f, 0.f, 0.f);
    arrCube[1].vNormal = Vec3(0.f, 1.f, 0.f);
    arrCube[1].vBinormal = Vec3(0.f, 0.f, -1.f);

    arrCube[2].vPos = Vec3(0.5f, 0.5f, -0.5f);
    arrCube[2].vColor = Vec4(1.f, 1.f, 1.f, 1.f);
    arrCube[2].vUV = Vec2(1.f, 1.f);
    arrCube[2].vTangent = Vec3(1.f, 0.f, 0.f);
    arrCube[2].vNormal = Vec3(0.f, 1.f, 0.f);
    arrCube[2].vBinormal = Vec3(0.f, 0.f, -1.f);

    arrCube[3].vPos = Vec3(-0.5f, 0.5f, -0.5f);
    arrCube[3].vColor = Vec4(1.f, 1.f, 1.f, 1.f);
    arrCube[3].vUV = Vec2(0.f, 1.f);
    arrCube[3].vTangent = Vec3(1.f, 0.f, 0.f);
    arrCube[3].vNormal = Vec3(0.f, 1.f, 0.f);
    arrCube[3].vBinormal = Vec3(0.f, 0.f, -1.f);


    // 아랫 면	
    arrCube[4].vPos = Vec3(-0.5f, -0.5f, -0.5f);
    arrCube[4].vColor = Vec4(1.f, 0.f, 0.f, 1.f);
    arrCube[4].vUV = Vec2(0.f, 0.f);
    arrCube[4].vTangent = Vec3(1.f, 0.f, 0.f);
    arrCube[4].vNormal = Vec3(0.f, -1.f, 0.f);
    arrCube[4].vBinormal = Vec3(0.f, 0.f, 1.f);

    arrCube[5].vPos = Vec3(0.5f, -0.5f, -0.5f);
    arrCube[5].vColor = Vec4(1.f, 0.f, 0.f, 1.f);
    arrCube[5].vUV = Vec2(1.f, 0.f);
    arrCube[5].vTangent = Vec3(1.f, 0.f, 0.f);
    arrCube[5].vNormal = Vec3(0.f, -1.f, 0.f);
    arrCube[5].vBinormal = Vec3(0.f, 0.f, 1.f);

    arrCube[6].vPos = Vec3(0.5f, -0.5f, 0.5f);
    arrCube[6].vColor = Vec4(1.f, 0.f, 0.f, 1.f);
    arrCube[6].vUV = Vec2(1.f, 1.f);
    arrCube[6].vTangent = Vec3(1.f, 0.f, 0.f);
    arrCube[6].vNormal = Vec3(0.f, -1.f, 0.f);
    arrCube[6].vBinormal = Vec3(0.f, 0.f, 1.f);

    arrCube[7].vPos = Vec3(-0.5f, -0.5f, 0.5f);
    arrCube[7].vColor = Vec4(1.f, 0.f, 0.f, 1.f);
    arrCube[7].vUV = Vec2(0.f, 1.f);
    arrCube[7].vTangent = Vec3(1.f, 0.f, 0.f);
    arrCube[7].vNormal = Vec3(0.f, -1.f, 0.f);
    arrCube[7].vBinormal = Vec3(0.f, 0.f, 1.f);

    // 왼쪽 면
    arrCube[8].vPos = Vec3(-0.5f, 0.5f, 0.5f);
    arrCube[8].vColor = Vec4(0.f, 1.f, 0.f, 1.f);
    arrCube[8].vUV = Vec2(0.f, 0.f);
    arrCube[8].vTangent = Vec3(0.f, 0.f, -1.f);
    arrCube[8].vNormal = Vec3(-1.f, 0.f, 0.f);
    arrCube[8].vBinormal = Vec3(0.f, -1.f, 0.f);

    arrCube[9].vPos = Vec3(-0.5f, 0.5f, -0.5f);
    arrCube[9].vColor = Vec4(0.f, 1.f, 0.f, 1.f);
    arrCube[9].vUV = Vec2(1.f, 0.f);
    arrCube[9].vTangent = Vec3(0.f, 0.f, -1.f);
    arrCube[9].vNormal = Vec3(-1.f, 0.f, 0.f);
    arrCube[9].vBinormal = Vec3(0.f, -1.f, 0.f);

    arrCube[10].vPos = Vec3(-0.5f, -0.5f, -0.5f);
    arrCube[10].vColor = Vec4(0.f, 1.f, 0.f, 1.f);
    arrCube[10].vUV = Vec2(1.f, 1.f);
    arrCube[10].vTangent = Vec3(0.f, 0.f, -1.f);
    arrCube[10].vNormal = Vec3(-1.f, 0.f, 0.f);
    arrCube[10].vBinormal = Vec3(0.f, -1.f, 0.f);

    arrCube[11].vPos = Vec3(-0.5f, -0.5f, 0.5f);
    arrCube[11].vColor = Vec4(0.f, 1.f, 0.f, 1.f);
    arrCube[11].vUV = Vec2(0.f, 1.f);
    arrCube[11].vTangent = Vec3(0.f, 0.f, -1.f);
    arrCube[11].vNormal = Vec3(-1.f, 0.f, 0.f);
    arrCube[11].vBinormal = Vec3(0.f, -1.f, 0.f);

    // 오른쪽 면
    arrCube[12].vPos = Vec3(0.5f, 0.5f, -0.5f);
    arrCube[12].vColor = Vec4(0.f, 0.f, 1.f, 1.f);
    arrCube[12].vUV = Vec2(0.f, 0.f);
    arrCube[12].vTangent = Vec3(0.f, 0.f, 1.f);
    arrCube[12].vNormal = Vec3(1.f, 0.f, 0.f);
    arrCube[12].vBinormal = Vec3(0.f, -1.f, 0.f);

    arrCube[13].vPos = Vec3(0.5f, 0.5f, 0.5f);
    arrCube[13].vColor = Vec4(0.f, 0.f, 1.f, 1.f);
    arrCube[13].vUV = Vec2(1.f, 0.f);
    arrCube[13].vTangent = Vec3(0.f, 0.f, 1.f);
    arrCube[13].vNormal = Vec3(1.f, 0.f, 0.f);
    arrCube[13].vBinormal = Vec3(0.f, -1.f, 0.f);

    arrCube[14].vPos = Vec3(0.5f, -0.5f, 0.5f);
    arrCube[14].vColor = Vec4(0.f, 0.f, 1.f, 1.f);
    arrCube[14].vUV = Vec2(1.f, 1.f);
    arrCube[14].vTangent = Vec3(0.f, 0.f, 1.f);
    arrCube[14].vNormal = Vec3(1.f, 0.f, 0.f);
    arrCube[14].vBinormal = Vec3(0.f, -1.f, 0.f);

    arrCube[15].vPos = Vec3(0.5f, -0.5f, -0.5f);
    arrCube[15].vColor = Vec4(0.f, 0.f, 1.f, 1.f);
    arrCube[15].vUV = Vec2(0.f, 1.f);
    arrCube[15].vTangent = Vec3(0.f, 0.f, 1.f);
    arrCube[15].vNormal = Vec3(1.f, 0.f, 0.f);
    arrCube[15].vBinormal = Vec3(0.f, -1.f, 0.f);

    // 뒷 면
    arrCube[16].vPos = Vec3(0.5f, 0.5f, 0.5f);
    arrCube[16].vColor = Vec4(1.f, 1.f, 0.f, 1.f);
    arrCube[16].vUV = Vec2(0.f, 0.f);
    arrCube[16].vTangent = Vec3(-1.f, 0.f, 0.f);
    arrCube[16].vNormal = Vec3(0.f, 0.f, 1.f);
    arrCube[16].vBinormal = Vec3(0.f, -1.f, 1.f);

    arrCube[17].vPos = Vec3(-0.5f, 0.5f, 0.5f);
    arrCube[17].vColor = Vec4(1.f, 1.f, 0.f, 1.f);
    arrCube[17].vUV = Vec2(1.f, 0.f);
    arrCube[17].vTangent = Vec3(-1.f, 0.f, 0.f);
    arrCube[17].vNormal = Vec3(0.f, 0.f, 1.f);
    arrCube[17].vBinormal = Vec3(0.f, -1.f, 1.f);

    arrCube[18].vPos = Vec3(-0.5f, -0.5f, 0.5f);
    arrCube[18].vColor = Vec4(1.f, 1.f, 0.f, 1.f);
    arrCube[18].vUV = Vec2(1.f, 1.f);
    arrCube[18].vTangent = Vec3(-1.f, 0.f, 0.f);
    arrCube[18].vNormal = Vec3(0.f, 0.f, 1.f);
    arrCube[18].vBinormal = Vec3(0.f, -1.f, 1.f);

    arrCube[19].vPos = Vec3(0.5f, -0.5f, 0.5f);
    arrCube[19].vColor = Vec4(1.f, 1.f, 0.f, 1.f);
    arrCube[19].vUV = Vec2(0.f, 1.f);
    arrCube[19].vTangent = Vec3(-1.f, 0.f, 0.f);
    arrCube[19].vNormal = Vec3(0.f, 0.f, 1.f);
    arrCube[19].vBinormal = Vec3(0.f, -1.f, 1.f);

    // 앞 면
    arrCube[20].vPos = Vec3(-0.5f, 0.5f, -0.5f);
    arrCube[20].vColor = Vec4(1.f, 0.f, 1.f, 1.f);
    arrCube[20].vUV = Vec2(0.f, 0.f);
    arrCube[20].vTangent = Vec3(1.f, 0.f, 0.f);
    arrCube[20].vNormal = Vec3(0.f, 0.f, -1.f);
    arrCube[20].vBinormal = Vec3(0.f, -1.f, 0.f);

    arrCube[21].vPos = Vec3(0.5f, 0.5f, -0.5f);
    arrCube[21].vColor = Vec4(1.f, 0.f, 1.f, 1.f);
    arrCube[21].vUV = Vec2(1.f, 0.f);
    arrCube[21].vTangent = Vec3(1.f, 0.f, 0.f);
    arrCube[21].vNormal = Vec3(0.f, 0.f, -1.f);
    arrCube[21].vBinormal = Vec3(0.f, -1.f, 0.f);

    arrCube[22].vPos = Vec3(0.5f, -0.5f, -0.5f);
    arrCube[22].vColor = Vec4(1.f, 0.f, 1.f, 1.f);
    arrCube[22].vUV = Vec2(1.f, 1.f);
    arrCube[22].vTangent = Vec3(1.f, 0.f, 0.f);
    arrCube[22].vNormal = Vec3(0.f, 0.f, -1.f);
    arrCube[22].vBinormal = Vec3(0.f, -1.f, 0.f);

    arrCube[23].vPos = Vec3(-0.5f, -0.5f, -0.5f);
    arrCube[23].vColor = Vec4(1.f, 0.f, 1.f, 1.f);
    arrCube[23].vUV = Vec2(0.f, 1.f);
    arrCube[23].vTangent = Vec3(1.f, 0.f, 0.f);
    arrCube[23].vNormal = Vec3(0.f, 0.f, -1.f);
    arrCube[23].vBinormal = Vec3(0.f, -1.f, 0.f);

    // 인덱스
    for (int i = 0; i < 12; i += 2)
    {
        vecIdx.push_back(i * 2);
        vecIdx.push_back(i * 2 + 1);
        vecIdx.push_back(i * 2 + 2);

        vecIdx.push_back(i * 2);
        vecIdx.push_back(i * 2 + 2);
        vecIdx.push_back(i * 2 + 3);
    }

    pMesh = new CMesh;
    pMesh->SetEngineAsset(true);
    pMesh->Create(arrCube, 24, vecIdx.data(), (UINT)vecIdx.size());
    CAssetMgr::GetInst()->AddAsset(L"EA_CubeMesh", pMesh.Get());


    vecIdx.clear();
    vecIdx.push_back(0); vecIdx.push_back(1); vecIdx.push_back(2);
    vecIdx.push_back(3); vecIdx.push_back(0); vecIdx.push_back(7);
    vecIdx.push_back(4); vecIdx.push_back(3); vecIdx.push_back(2);
    vecIdx.push_back(5); vecIdx.push_back(4); vecIdx.push_back(7);
    vecIdx.push_back(6); vecIdx.push_back(5); vecIdx.push_back(6);
    vecIdx.push_back(1);

    pMesh = new CMesh;
    pMesh->SetEngineAsset(true);
    pMesh->Create(arrCube, 24, vecIdx.data(), (UINT)vecIdx.size());
    CAssetMgr::GetInst()->AddAsset(L"EA_CubeMesh_Debug", pMesh.Get());


    // ============
    // Sphere Mesh
    // ============
    vecVtx.clear();
    vecIdx.clear();

    float fRadius = 0.5f;

    // Top
    v.vPos = Vec3(0.f, fRadius, 0.f);
    v.vUV = Vec2(0.5f, 0.f);
    v.vColor = Vec4(1.f, 1.f, 1.f, 1.f);
    v.vNormal = v.vPos;
    v.vNormal.Normalize();
    v.vTangent = Vec3(1.f, 0.f, 0.f);
    v.vBinormal = Vec3(0.f, 0.f, -1.f);
    vecVtx.push_back(v);

    // Body
    UINT iStackCount = 40; // 가로 분할 개수
    UINT iSliceCount = 40; // 세로 분할 개수

    float fStackAngle = XM_PI / iStackCount;
    float fSliceAngle = XM_2PI / iSliceCount;

    float fUVXStep = 1.f / (float)iSliceCount;
    float fUVYStep = 1.f / (float)iStackCount;

    for (UINT i = 1; i < iStackCount; ++i)
    {
        float phi = i * fStackAngle;

        for (UINT j = 0; j <= iSliceCount; ++j)
        {
            float theta = j * fSliceAngle;

            v.vPos = Vec3(fRadius * sinf(i * fStackAngle) * cosf(j * fSliceAngle)
                , fRadius * cosf(i * fStackAngle)
                , fRadius * sinf(i * fStackAngle) * sinf(j * fSliceAngle));

            v.vUV = Vec2(fUVXStep * j, fUVYStep * i);
            v.vColor = Vec4(1.f, 1.f, 1.f, 1.f);
            v.vNormal = v.vPos;
            v.vNormal.Normalize();

            v.vTangent.x = -fRadius * sinf(phi) * sinf(theta);
            v.vTangent.y = 0.f;
            v.vTangent.z = fRadius * sinf(phi) * cosf(theta);
            v.vTangent.Normalize();

            v.vNormal.Cross(v.vTangent, v.vBinormal);
            v.vBinormal.Normalize();

            vecVtx.push_back(v);
        }
    }

    // Bottom
    v.vPos = Vec3(0.f, -fRadius, 0.f);
    v.vUV = Vec2(0.5f, 1.f);
    v.vColor = Vec4(1.f, 1.f, 1.f, 1.f);
    v.vNormal = v.vPos;
    v.vNormal.Normalize();

    v.vTangent = Vec3(1.f, 0.f, 0.f);
    v.vBinormal = Vec3(0.f, 0.f, -1.f);
    vecVtx.push_back(v);

    // 인덱스
    // 북극점
    for (UINT i = 0; i < iSliceCount; ++i)
    {
        vecIdx.push_back(0);
        vecIdx.push_back(i + 2);
        vecIdx.push_back(i + 1);
    }

    // 몸통
    for (UINT i = 0; i < iStackCount - 2; ++i)
    {
        for (UINT j = 0; j < iSliceCount; ++j)
        {
            // + 
            // | \
			// +--+
            vecIdx.push_back((iSliceCount + 1) * (i)+(j)+1);
            vecIdx.push_back((iSliceCount + 1) * (i + 1) + (j + 1) + 1);
            vecIdx.push_back((iSliceCount + 1) * (i + 1) + (j)+1);

            // +--+
            //  \ |
            //    +
            vecIdx.push_back((iSliceCount + 1) * (i)+(j)+1);
            vecIdx.push_back((iSliceCount + 1) * (i)+(j + 1) + 1);
            vecIdx.push_back((iSliceCount + 1) * (i + 1) + (j + 1) + 1);
        }
    }

    // 남극점
    UINT iBottomIdx = (UINT)vecVtx.size() - 1;
    for (UINT i = 0; i < iSliceCount; ++i)
    {
        vecIdx.push_back(iBottomIdx);
        vecIdx.push_back(iBottomIdx - (i + 2));
        vecIdx.push_back(iBottomIdx - (i + 1));
    }

    pMesh = new CMesh;
    pMesh->SetEngineAsset(true);
    pMesh->Create(vecVtx.data(), (UINT)vecVtx.size(), vecIdx.data(), (UINT)vecIdx.size());
    AddAsset(L"EA_SphereMesh", pMesh.Get());
    vecVtx.clear();
    vecIdx.clear();
}

void CAssetMgr::CreateEngineGraphicShader()
{
	// 표준 쉐이더 생성
	AssetPtr<CGraphicShader>	pShader = nullptr;

	// ===============
	// Deferred Shader
	// ===============
	pShader = new CGraphicShader;

	pShader->CreateVertexShader(L"HLSL\\Engine\\deferred.fx", "VS_Deferred");
	pShader->CreatePixelShader(L"HLSL\\Engine\\deferred.fx", "PS_Deferred");
	pShader->SetDomain(SHADER_DOMAIN::DOMAIN_MASKED);
	pShader->SetRSType(RS_TYPE::CULL_BACK);
	pShader->SetDSType(DS_TYPE::LESS);
	pShader->SetBSType(BS_TYPE::DEFAULT);
	pShader->SetTexData(TEX_0, "Main Texture");
    pShader->SetTexData(TEX_1, "Normal Texture");
	pShader->SetEngineAsset(true);

	AddAsset(L"EA_DeferredShader", pShader.Get());


	// ==========================
	// Deferred AlphaBlend Shader
	// ==========================
	pShader = new CGraphicShader;

	pShader->CreateVertexShader(L"HLSL\\Engine\\deferred.fx", "VS_Deferred");
	pShader->CreatePixelShader(L"HLSL\\Engine\\deferred.fx", "PS_Deferred_AlphaBlend");
	pShader->SetDomain(SHADER_DOMAIN::DOMAIN_TRANSPARENT);
	pShader->SetRSType(RS_TYPE::CULL_BACK);
	pShader->SetDSType(DS_TYPE::NO_WRITE);
	pShader->SetBSType(BS_TYPE::ALPHABLEND);
    pShader->SetTexData(TEX_0, "Main Texture");
    pShader->SetTexData(TEX_1, "Normal Texture");
	pShader->SetConstData(FLOAT_0, "Alpha Tint");
	pShader->SetEngineAsset(true);

	AddAsset(L"EA_DeferredAlphaBlendShader", pShader.Get());


	// Flipbook Shader
	pShader = new CGraphicShader;
	pShader->CreateVertexShader(L"HLSL\\Engine\\flipbook.fx", "VS_Flipbook");
	pShader->CreatePixelShader(L"HLSL\\Engine\\flipbook.fx", "PS_Flipbook");
	pShader->SetRSType(RS_TYPE::CULL_NONE);
	pShader->SetDomain(SHADER_DOMAIN::DOMAIN_TRANSPARENT);
	pShader->SetBSType(BS_TYPE::ALPHABLEND);
	pShader->SetRSType(RS_TYPE::CULL_NONE);
	pShader->SetDSType(DS_TYPE::NO_WRITE);
	pShader->SetTexData(TEX_0, "Atlas Texture");
	pShader->SetConstData(INT_0, "Atlas Tex Flag");
	pShader->SetConstData(INT_1, "Single Tex Flag");
	pShader->SetConstData(VEC2_0, "LeftTop");
	pShader->SetConstData(VEC2_1, "Slice");
	pShader->SetConstData(VEC2_2, "Background");
	pShader->SetConstData(VEC2_3, "Offset");
	pShader->SetEngineAsset(true);
	AddAsset(L"EA_FlipbookShader", pShader.Get());

	// Sprite Shader
	pShader = new CGraphicShader;
	pShader->CreateVertexShader(L"HLSL\\Engine\\sprite.fx", "VS_Sprite");
	pShader->CreatePixelShader(L"HLSL\\Engine\\sprite.fx", "PS_Sprite");
	pShader->SetRSType(RS_TYPE::CULL_NONE);
	pShader->SetDomain(SHADER_DOMAIN::DOMAIN_MASKED);
	pShader->SetEngineAsset(true); 
	AddAsset(L"EA_SpriteShader", pShader.Get());


	// TileRender Shader
	pShader = new CGraphicShader;
	pShader->CreateVertexShader(L"HLSL\\Engine\\tilerender.fx", "VS_TileRender");
	pShader->CreatePixelShader(L"HLSL\\Engine\\tilerender.fx", "PS_TileRender");
	pShader->SetBSType(BS_TYPE::ALPHABLEND);
	pShader->SetRSType(RS_TYPE::CULL_NONE);
	pShader->SetDSType(DS_TYPE::NO_WRITE);
	pShader->SetDomain(SHADER_DOMAIN::DOMAIN_TRANSPARENT);
	pShader->SetEngineAsset(true); 
	AddAsset(L"EA_TileRenderShader", pShader.Get());


	// UI Shader
	pShader = new CGraphicShader;
	pShader->CreateVertexShader(L"HLSL\\Engine\\ui.fx", "VS_UI");
	pShader->CreateGeometryShader(L"HLSL\\Engine\\ui.fx", "GS_UI");
	pShader->CreatePixelShader(L"HLSL\\Engine\\ui.fx", "PS_UI");
	pShader->SetBSType(BS_TYPE::ALPHABLEND);
	pShader->SetRSType(RS_TYPE::CULL_NONE);
	pShader->SetDSType(DS_TYPE::NO_WRITE);
	pShader->SetDomain(SHADER_DOMAIN::DOMAIN_TRANSPARENT);
	pShader->SetTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
	pShader->SetTexData(TEX_0, "UI Texture");
	pShader->SetConstData(FLOAT_0, "Alpha Tint");
	pShader->SetConstData(FLOAT_1, "Scale X");
	pShader->SetConstData(FLOAT_2, "Scale Y");
	pShader->SetConstData(VEC2_0, "Screen Pos (NDC)");
	pShader->SetEngineAsset(true);
	AddAsset(L"EA_UIShader", pShader.Get());


    // Light Shader
    pShader = new CGraphicShader;
    pShader->CreateVertexShader(L"HLSL\\Engine\\light.fx", "VS_Light");
    pShader->CreatePixelShader(L"HLSL\\Engine\\light.fx", "PS_Light");
    pShader->SetBSType(BS_TYPE::ONE_ONE);
    pShader->SetRSType(RS_TYPE::CULL_NONE);
    pShader->SetDSType(DS_TYPE::NO_TEST_NO_WIRITE);
    pShader->SetDomain(SHADER_DOMAIN::DOMAIN_SYSTEM);
    pShader->SetEngineAsset(true);
    CAssetMgr::GetInst()->AddAsset(L"EA_LightShader", pShader.Get());


    // Sprite Shader
    pShader = new CGraphicShader;
    pShader->CreateVertexShader(L"HLSL\\Engine\\sprite.fx", "VS_Sprite");
    pShader->CreatePixelShader(L"HLSL\\Engine\\sprite.fx", "PS_Sprite");
    pShader->SetRSType(RS_TYPE::CULL_NONE);
    pShader->SetDomain(SHADER_DOMAIN::DOMAIN_MASKED);
    pShader->SetEngineAsset(true);
    CAssetMgr::GetInst()->AddAsset(L"EA_SpriteShader", pShader.Get());


    // Merge Shader
    pShader = new CGraphicShader;
    pShader->CreateVertexShader(L"HLSL\\Engine\\merge.fx", "VS_Merge");
    pShader->CreatePixelShader(L"HLSL\\Engine\\merge.fx", "PS_Merge");
    pShader->SetBSType(BS_TYPE::DEFAULT);
    pShader->SetRSType(RS_TYPE::CULL_NONE);
    pShader->SetDSType(DS_TYPE::NO_WRITE);
    pShader->SetDomain(SHADER_DOMAIN::DOMAIN_SYSTEM);
    pShader->SetEngineAsset(true);
    CAssetMgr::GetInst()->AddAsset(L"EA_MergeShader", pShader.Get());


    // Particle Shader
    pShader = new CGraphicShader;
    pShader->CreateVertexShader(L"HLSL\\Engine\\particle.fx", "VS_Particle");
    pShader->CreateGeometryShader(L"HLSL\\Engine\\particle.fx", "GS_Particle");
    pShader->CreatePixelShader(L"HLSL\\Engine\\particle.fx", "PS_Particle");
    pShader->SetDomain(SHADER_DOMAIN::DOMAIN_TRANSPARENT);
    pShader->SetBSType(BS_TYPE::ALPHABLEND);
    pShader->SetRSType(RS_TYPE::CULL_NONE);
    pShader->SetDSType(DS_TYPE::NO_WRITE);
    pShader->SetTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
    pShader->SetEngineAsset(true);
    CAssetMgr::GetInst()->AddAsset(L"EA_ParticleShader", pShader.Get());


    // Skybox Shader
    pShader = new CGraphicShader;
    pShader->CreateVertexShader(L"HLSL\\Engine\\skybox.fx", "VS_SkyBox");
    pShader->CreatePixelShader(L"HLSL\\Engine\\skybox.fx", "PS_SkyBox");
    pShader->SetBSType(BS_TYPE::ALPHABLEND);
    pShader->SetRSType(RS_TYPE::CULL_FRONT);
    pShader->SetDSType(DS_TYPE::LESS_EQUAL);
    pShader->SetDomain(SHADER_DOMAIN::DOMAIN_EFFECT);
    pShader->SetEngineAsset(true);
    AddAsset(L"EA_SkyboxShader", pShader.Get());
}

void CAssetMgr::CreateEngineMaterial()
{
	// EA_CursorMtrl
	AssetPtr<CMaterial> pMaterial = new CMaterial;
	pMaterial->SetEngineAsset(true);
	pMaterial->SetShader(CAssetMgr::GetInst()->Load<CGraphicShader>(L"EA_UIShader", true));
	pMaterial->SetConstParam(FLOAT_0, 1.f);
	AddAsset(L"EA_CursorMtrl", pMaterial.Get());


	// EA_FlipbookMtrl
	pMaterial = new CMaterial;
	pMaterial->SetEngineAsset(true);
	pMaterial->SetShader(CAssetMgr::GetInst()->Load<CGraphicShader>(L"EA_FlipbookShader", true));
	AddAsset(L"EA_FlipbookMtrl", pMaterial.Get());


    // Light Material
    pMaterial = new CMaterial;
    pMaterial->SetEngineAsset(true);
    pMaterial->SetShader(CAssetMgr::GetInst()->Load<CGraphicShader>(L"EA_LightShader", true));
    CAssetMgr::GetInst()->AddAsset(L"EA_LightMtrl", pMaterial.Get());

    // SpriteMtrl
    pMaterial = new CMaterial;
    pMaterial->SetEngineAsset(true);
    pMaterial->SetShader(CAssetMgr::GetInst()->Load<CGraphicShader>(L"EA_SpriteShader", true));
    CAssetMgr::GetInst()->AddAsset(L"EA_SpriteMtrl", pMaterial.Get());


    // Merge Material
    pMaterial = new CMaterial;
    pMaterial->SetEngineAsset(true);
    pMaterial->SetShader(CAssetMgr::GetInst()->Load<CGraphicShader>(L"EA_MergeShader", true));
    CAssetMgr::GetInst()->AddAsset(L"EA_MergeMtrl", pMaterial.Get());


    // Particle Material
    pMaterial = new CMaterial;
    pMaterial->SetEngineAsset(true);
    pMaterial->SetShader(CAssetMgr::GetInst()->Load<CGraphicShader>(L"EA_ParticleShader", true));
    CAssetMgr::GetInst()->AddAsset(L"EA_ParticleMtrl", pMaterial.Get());


    // SkyBox Material
    pMaterial = new CMaterial;
    pMaterial->SetEngineAsset(true);
    pMaterial->SetShader(CAssetMgr::GetInst()->Load<CGraphicShader>(L"EA_SkyboxShader", true));
    CAssetMgr::GetInst()->AddAsset(L"EA_SkyboxMtrl", pMaterial.Get());
}

