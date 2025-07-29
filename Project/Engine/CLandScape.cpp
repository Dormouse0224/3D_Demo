#include "pch.h"
#include "CLandScape.h"

#include "CKeyMgr.h"
#include "CRenderMgr.h"
#include "CAssetMgr.h"

#include "CTransform.h"
#include "CCamera.h"

CLandScape::CLandScape()
	: CRenderComponent(COMPONENT_TYPE::LANDSCAPE)
	, m_FaceX(1)
	, m_FaceZ(1)
    , m_ColorTex(nullptr)
    , m_NormalTex(nullptr)
    , m_HeightMap(nullptr)
    , m_HeightMapCS(nullptr)
    , m_RaycastCS(nullptr)
    , m_RaycastOut(nullptr)
    , m_Out{}
    , m_vecBrush{}
	, m_BrushScale(Vec2(0.5f, 0.5f))
	, m_BrushIdx(0)
    , m_WeightMap(nullptr)
    , m_WeightWidth(1024)
    , m_WeightHeight(1024)
    , m_WeightMapCS(nullptr)
    , m_WeightIdx(0)
	, m_MinLevel(0.f)
	, m_MaxLevel(4.f)
	, m_MaxLevelRange(2000.f)
	, m_MinLevelRange(6000.f)
	, m_Mode(LANDSCAPE_MODE::NONE)
{
	CreateLandScapeMesh();

    SetMaterial(CAssetMgr::GetInst()->FindAsset<CMaterial>(L"EA_LandScapeMtrl"), 0);

	CreateComputeShader();
    CreateWeightMap();
    CreateHeightMap(1024, 1024);

	// Raycasting 결과를 받는 용도의 구조화버퍼
	m_RaycastOut = new CStructuredBuffer;
	m_RaycastOut->Create(sizeof(tRaycastOut), 1, SB_TYPE::SRV_UAV, true);

	// BrushTexture 추가	
	AddBrushTexture(CAssetMgr::GetInst()->FindAsset<CTexture>(L"Texture\\brush\\TX_GlowScene_2.png"));
	AddBrushTexture(CAssetMgr::GetInst()->FindAsset<CTexture>(L"Texture\\brush\\TX_HitFlash_0.png"));
	AddBrushTexture(CAssetMgr::GetInst()->FindAsset<CTexture>(L"Texture\\brush\\TX_HitFlash02.png"));
	AddBrushTexture(CAssetMgr::GetInst()->FindAsset<CTexture>(L"Texture\\brush\\TX_Twirl02.png"));
	AddBrushTexture(CAssetMgr::GetInst()->FindAsset<CTexture>(L"Texture\\brush\\FX_Flare.png"));

}

CLandScape::CLandScape(const CLandScape& _Other)
    : CRenderComponent(COMPONENT_TYPE::LANDSCAPE)
    , m_FaceX(_Other.m_FaceX)
    , m_FaceZ(_Other.m_FaceZ)
    , m_ColorTex(_Other.m_ColorTex)
    , m_NormalTex(_Other.m_NormalTex)
    , m_HeightMap(nullptr)
    , m_HeightMapCS(nullptr)
    , m_RaycastCS(nullptr)
    , m_RaycastOut(nullptr)
    , m_Out{}
    , m_vecBrush{}
    , m_BrushScale(_Other.m_BrushScale)
    , m_BrushIdx(_Other.m_BrushIdx)
    , m_WeightMap(_Other.m_WeightMap->Clone())
    , m_WeightWidth(1024)
    , m_WeightHeight(1024)
    , m_WeightMapCS(nullptr)
    , m_WeightIdx(0)
    , m_MinLevel(_Other.m_MinLevel)
    , m_MaxLevel(_Other.m_MaxLevel)
    , m_MaxLevelRange(_Other.m_MaxLevelRange)
    , m_MinLevelRange(_Other.m_MinLevelRange)
    , m_Mode(LANDSCAPE_MODE::NONE)
{
    CreateLandScapeMesh();

    SetMaterial(CAssetMgr::GetInst()->FindAsset<CMaterial>(L"EA_LandScapeMtrl"), 0);

    CreateComputeShader();
    //CreateWeightMap();
    //vector<tWeight8> Data(m_WeightWidth * m_WeightHeight);
    //_Other.m_WeightMap->GetData(Data.data(), sizeof(tWeight8), m_WeightWidth * m_WeightHeight);
    //m_WeightMap->SetData(Data.data(), sizeof(tWeight8), m_WeightWidth * m_WeightHeight);
    CreateHeightMap(_Other.m_HeightMap);

    // Raycasting 결과를 받는 용도의 구조화버퍼
    m_RaycastOut = new CStructuredBuffer;
    m_RaycastOut->Create(sizeof(tRaycastOut), 1, SB_TYPE::SRV_UAV, true);

    // BrushTexture 추가	
    AddBrushTexture(CAssetMgr::GetInst()->FindAsset<CTexture>(L"Texture\\brush\\TX_GlowScene_2.png"));
    AddBrushTexture(CAssetMgr::GetInst()->FindAsset<CTexture>(L"Texture\\brush\\TX_HitFlash_0.png"));
    AddBrushTexture(CAssetMgr::GetInst()->FindAsset<CTexture>(L"Texture\\brush\\TX_HitFlash02.png"));
    AddBrushTexture(CAssetMgr::GetInst()->FindAsset<CTexture>(L"Texture\\brush\\TX_Twirl02.png"));
    AddBrushTexture(CAssetMgr::GetInst()->FindAsset<CTexture>(L"Texture\\brush\\FX_Flare.png"));

}

CLandScape::~CLandScape()
{
	if (nullptr != m_RaycastOut)
    {
        delete m_RaycastOut;
        m_RaycastOut = nullptr;
    }

	if (nullptr != m_WeightMap)
    {
        delete m_WeightMap;
        m_WeightMap = nullptr;
    }
}

void CLandScape::FinalTick()
{
	//// 모드 전환
	//if (KEY_TAP(Keyboard::NUM_6))
	//{
 //       UINT next = (UINT)m_Mode + 1;
 //       if (next == (UINT)LANDSCAPE_MODE::END)
 //           m_Mode = (LANDSCAPE_MODE)0;
	//	else
 //           m_Mode = (LANDSCAPE_MODE)next;
	//}

	//// 브러쉬 바꾸기
	//if (KEY_TAP(Keyboard::NUM_7))
	//{
	//	++m_BrushIdx;
	//	if (m_vecBrush.size() <= m_BrushIdx)
	//		m_BrushIdx = 0;
	//}

	//// 가중치 인덱스 바꾸기
	//if (KEY_TAP(Keyboard::NUM_8))
	//{
	//	++m_WeightIdx;
	//	if (m_ColorTex->GetDesc().ArraySize <= (UINT)m_WeightIdx)
	//		m_WeightIdx = 0;
	//}

	if (Raycasting() && KEY_PRESSED(Keyboard::MOUSE_LBTN))
	{
		if (LANDSCAPE_MODE::HEIGHTMAP == m_Mode)
		{
			if (nullptr != m_HeightMap)
			{
				// 높이맵 설정
				m_HeightMapCS->SetBrushPos(m_RaycastOut);
				m_HeightMapCS->SetBrushScale(m_BrushScale);
				m_HeightMapCS->SetHeightMap(m_HeightMap);
				m_HeightMapCS->SetBrushTex(m_vecBrush[m_BrushIdx]);
				m_HeightMapCS->Execute();
			}
		}
		else if (LANDSCAPE_MODE::SPLATING == m_Mode)
		{
			if (m_WeightWidth != 0 && m_WeightHeight != 0)
			{
				m_WeightMapCS->SetBrushPos(m_RaycastOut);
				m_WeightMapCS->SetBrushScale(m_BrushScale);
				m_WeightMapCS->SetBrushTex(m_vecBrush[m_BrushIdx]);
				m_WeightMapCS->SetWeightMap(m_WeightMap);
				m_WeightMapCS->SetWeightIdx(m_WeightIdx);
				m_WeightMapCS->SetWeightMapWidthHeight(m_WeightWidth, m_WeightHeight);
				m_WeightMapCS->Execute();
			}
		}
	}
}

void CLandScape::Render()
{
	Transform()->Binding();

	//GetMaterial(0)->GetShader()->SetRSType(RS_TYPE::WIRE_FRAME);

	//GetMaterial()->SetConstParam(FLOAT_0, 10.f);
	GetMaterial()->SetConstParam(INT_0, m_FaceX);
	GetMaterial()->SetConstParam(INT_1, m_FaceZ);
	GetMaterial()->SetConstParam(INT_2, (int)m_Mode);  // 지형 모드

	GetMaterial()->SetTexParam(TEX_0, m_HeightMap);
	GetMaterial()->SetTexParam(TEXARR_0, m_ColorTex);  // 지형 색상 텍스쳐
	GetMaterial()->SetTexParam(TEXARR_1, m_NormalTex); // 지형 노말 텍스쳐
    if (m_ColorTex.Get())
	    GetMaterial()->SetConstParam(INT_3, (int)m_ColorTex->GetDesc().ArraySize); // 텍스쳐 배열 개수
    else
        GetMaterial()->SetConstParam(INT_3, 0);

    GetMaterial()->SetConstParam(FLOAT_1, Transform()->GetRelativeScale().y);

	GetMaterial()->SetTexParam(TEX_1, m_vecBrush[m_BrushIdx]);		// Brush 텍스쳐
	GetMaterial()->SetConstParam(VEC2_0, m_BrushScale);			// Brush 크기
	GetMaterial()->SetConstParam(VEC2_1, m_Out.Location);			// RayCast 위치
	GetMaterial()->SetConstParam(FLOAT_0, (float)m_Out.Success);	// RayCast 성공여부

	// 테셀레이션 레벨
	GetMaterial()->SetConstParam(VEC4_0, Vec4(m_MinLevel, m_MaxLevel, m_MinLevelRange, m_MaxLevelRange));

	// 카메라 월드 위치
	CCamera* pCam = CRenderMgr::GetInst()->GetCurrentCam();
	GetMaterial()->SetConstParam(VEC4_1, pCam->GetOwner()->Transform()->GetWorldTrans());

	m_WeightMap->Binding(20); // WeightMap t20 바인딩
	GetMaterial()->SetConstParam(VEC2_2, Vec2(m_WeightWidth, m_WeightHeight)); // 가중치 해상도

	GetMaterial()->Binding();
	GetMesh()->Render(0);

    GetMaterial()->Unbind();
	m_WeightMap->Unbind(); // WeightMap 버퍼 바인딩 클리어
}

int CLandScape::Save(fstream& _Stream)
{
    // 그리드 정보 작성
    _Stream.write(reinterpret_cast<char*>(&m_FaceX), sizeof(UINT));
    _Stream.write(reinterpret_cast<char*>(&m_FaceZ), sizeof(UINT));

    // 컬러 및 노말 텍스쳐 정보 작성
    SaveWString(m_ColorTex->GetName(), _Stream);
    SaveWString(m_NormalTex->GetName(), _Stream);

    // 높이맵을 파일로 저장 후 높이맵 정보 작성
    wstring heightmapName = L"Landscape\\HM_" + to_wstring(GetID());
    m_HeightMap->Save(heightmapName);
    SaveWString(L"Texture\\" + heightmapName + L".dds", _Stream);

    // 가중치맵 메타데이터 작성 후, 가중치맵 데이터를 받아와 작성
    _Stream.write(reinterpret_cast<char*>(&m_WeightWidth), sizeof(UINT));
    _Stream.write(reinterpret_cast<char*>(&m_WeightHeight), sizeof(UINT));
    vector<tWeight8> Data(m_WeightWidth * m_WeightHeight);
    m_WeightMap->GetData(Data.data(), sizeof(tWeight8), m_WeightWidth * m_WeightHeight);
    _Stream.write(reinterpret_cast<char*>(Data.data()), sizeof(tWeight8) * m_WeightWidth * m_WeightHeight);

    // 테셀레이션 정보 작성
    _Stream.write(reinterpret_cast<char*>(&m_MinLevel), sizeof(float));
    _Stream.write(reinterpret_cast<char*>(&m_MaxLevel), sizeof(float));
    _Stream.write(reinterpret_cast<char*>(&m_MaxLevelRange), sizeof(float));
    _Stream.write(reinterpret_cast<char*>(&m_MinLevelRange), sizeof(float));

    return S_OK;
}

int CLandScape::Load(fstream& _Stream)
{
    // 그리드 정보 불러오기
    _Stream.read(reinterpret_cast<char*>(&m_FaceX), sizeof(UINT));
    _Stream.read(reinterpret_cast<char*>(&m_FaceZ), sizeof(UINT));

    // 컬러 및 노말 텍스쳐 정보 불러오기
    wstring texName;
    LoadWString(texName, _Stream);
    m_ColorTex = CAssetMgr::GetInst()->Load<CTexture>(texName);
    LoadWString(texName, _Stream);
    m_NormalTex = CAssetMgr::GetInst()->Load<CTexture>(texName);

    // 높이맵을 파일로 저장 후 높이맵 정보 작성
    wstring heightmapName;
    LoadWString(heightmapName, _Stream);
    m_HeightMap = CAssetMgr::GetInst()->Load<CTexture>(heightmapName);
    m_HeightMap->CreateUAV();

    // 가중치맵 메타데이터 작성 후, 가중치맵 데이터를 받아와 작성
    _Stream.read(reinterpret_cast<char*>(&m_WeightWidth), sizeof(UINT));
    _Stream.read(reinterpret_cast<char*>(&m_WeightHeight), sizeof(UINT));
    vector<tWeight8> Data(m_WeightWidth * m_WeightHeight);
    _Stream.read(reinterpret_cast<char*>(Data.data()), sizeof(tWeight8) * m_WeightWidth * m_WeightHeight);
    CreateWeightMap();
    m_WeightMap->SetData(Data.data(), sizeof(tWeight8), m_WeightWidth * m_WeightHeight);

    // 테셀레이션 정보 작성
    _Stream.read(reinterpret_cast<char*>(&m_MinLevel), sizeof(float));
    _Stream.read(reinterpret_cast<char*>(&m_MaxLevel), sizeof(float));
    _Stream.read(reinterpret_cast<char*>(&m_MaxLevelRange), sizeof(float));
    _Stream.read(reinterpret_cast<char*>(&m_MinLevelRange), sizeof(float));

    return S_OK;
}

void CLandScape::SetFace(UINT _X, UINT _Z)
{
	if (m_FaceX == _X && m_FaceZ == _Z)
		return;

	m_FaceX = _X;
	m_FaceZ = _Z;

	CreateLandScapeMesh();
}

void CLandScape::SetHeightMap(AssetPtr<CTexture> _HeightMap)
{
	m_HeightMap = _HeightMap;
}

void CLandScape::SetMode(LANDSCAPE_MODE _Mode)
{
    if ((int)_Mode < (int)LANDSCAPE_MODE::END)
        m_Mode = _Mode;
    else
        m_Mode = LANDSCAPE_MODE::NONE;
}

void CLandScape::SetBrushIdx(UINT _Idx)
{
    if (m_vecBrush.size() > _Idx)
        m_BrushIdx = _Idx;
    else
        m_BrushIdx = 0;
}

void CLandScape::SetWeightIdx(int _Idx)
{
    if (m_ColorTex.Get() == nullptr)
        m_WeightIdx = 0;

    if (m_ColorTex->GetDesc().ArraySize > _Idx)
        m_WeightIdx = _Idx;
    else
        m_WeightIdx = 0;
}


int CLandScape::Raycasting()
{
    if (m_HeightMap.Get() == nullptr)
        return false;

	// 현재 시점 카메라 가져오기
	CCamera* pCam = CRenderMgr::GetInst()->GetCurrentCam();
	if (nullptr == pCam)
		return false;

	// 구조화버퍼 클리어
	m_Out = {};
	m_Out.Distance = 0xffffffff;
	m_RaycastOut->SetData(&m_Out);


	// 카메라가 시점에서 마우스를 향하는 Ray 정보를 가져옴
	tRay ray = pCam->GetRay();

	// LandScape 의 WorldInv 행렬 가져옴
    const Matrix& matWorldInv = XMMatrixInverse(NULL, Transform()->GetWorldMat());

	// 월드 기준 Ray 정보를 LandScape 의 Local 공간으로 데려감
	ray.vStart = XMVector3TransformCoord(ray.vStart, matWorldInv);
	ray.vDir = XMVector3TransformNormal(ray.vDir, matWorldInv);
	ray.vDir.Normalize();

	// Raycast 컴퓨트 쉐이더에 필요한 데이터 전달
	m_RaycastCS->SetRayInfo(ray);
	m_RaycastCS->SetFace(m_FaceX, m_FaceZ);
	m_RaycastCS->SetOutBuffer(m_RaycastOut);
	m_RaycastCS->SetHeightMap(m_HeightMap);

	// 컴퓨트쉐이더 실행
	m_RaycastCS->Execute();

	// 결과 확인
	m_RaycastOut->GetData(&m_Out);

	// 결과 확인
	if (m_Out.Success)
	{
		m_Out.Location;
		int a = 0;
	}

	return m_Out.Success;
}