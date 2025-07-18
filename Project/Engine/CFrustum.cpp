#include "pch.h"
#include "CFrustum.h"

#include "CCamera.h"

CFrustum::CFrustum()
	: m_Owner(nullptr)
	, m_ProjPos{}
{	
	//   4 -- 5
	//  /|   /|
	// 0 -- 1 6 
	// |    |/
	// 3 -- 2 
	// 투영공간(NDC) 좌표계의 시야 최대범위 위치
	m_ProjPos[0] = Vec3(-1.f, 1.f, 0.f);
	m_ProjPos[1] = Vec3(1.f, 1.f, 0.f);
	m_ProjPos[2] = Vec3(1.f, -1.f, 0.f);
	m_ProjPos[3] = Vec3(-1.f, -1.f, 0.f);

	m_ProjPos[4] = Vec3(-1.f, 1.f, 1.f);
	m_ProjPos[5] = Vec3(1.f, 1.f, 1.f);
	m_ProjPos[6] = Vec3(1.f, -1.f, 1.f);
	m_ProjPos[7] = Vec3(-1.f, -1.f, 1.f);
}

CFrustum::~CFrustum()
{
}

void CFrustum::FinalTick()
{
	// 뷰, 투영 역행렬
	const Matrix& matViewInv = XMMatrixInverse(NULL, m_Owner->GetViewMat());
	const Matrix& matProjInv = XMMatrixInverse(NULL, m_Owner->GetProjMat());
	Matrix matPVInv = matProjInv* matViewInv;

	// 월드상에서의 카메라 시야 범위
	Vec3 vWorldPos[8];

	// 카메라의 시야범위를 절두체(18개의 평면) 로 만든다.
	for (int i = 0; i < 8; ++i)
	{
		vWorldPos[i] = XMVector3TransformCoord(m_ProjPos[i], matPVInv);
	}

	// 월드 기준, 6개의 절두체 평면을 생성
	//   4 -- 5
	//  /|   /|
	// 0 -- 1 6 
	// |    |/
	// 3 -- 2 
    // XMPlaneFromPoints : 평면의 방정식 계수를 반환함.
    m_Face[(UINT)FACE_TYPE::eLEFT] = XMPlaneFromPoints(vWorldPos[4], vWorldPos[0], vWorldPos[7]);
    m_Face[(UINT)FACE_TYPE::eRIGHT] = XMPlaneFromPoints(vWorldPos[1], vWorldPos[5], vWorldPos[6]);
    m_Face[(UINT)FACE_TYPE::eBOTTOM] = XMPlaneFromPoints(vWorldPos[2], vWorldPos[6], vWorldPos[7]);
    m_Face[(UINT)FACE_TYPE::eTOP] = XMPlaneFromPoints(vWorldPos[1], vWorldPos[4], vWorldPos[5]);

	m_Face[(UINT)FACE_TYPE::eNEAR_0] = XMPlaneFromPoints(vWorldPos[0], vWorldPos[1], vWorldPos[2]);
    m_Face[(UINT)FACE_TYPE::eFAR_0] = XMPlaneFromPoints((vWorldPos[5] - vWorldPos[1]) * 0.01f + vWorldPos[1], (vWorldPos[4] - vWorldPos[0]) * 0.01f + vWorldPos[0], (vWorldPos[7] - vWorldPos[3]) * 0.01f + vWorldPos[3]);

    m_Face[(UINT)FACE_TYPE::eNEAR_1] = XMPlaneFromPoints((vWorldPos[4] - vWorldPos[0]) * 0.01f + vWorldPos[0], (vWorldPos[5] - vWorldPos[1]) * 0.01f + vWorldPos[1], (vWorldPos[7] - vWorldPos[3]) * 0.01f + vWorldPos[3]);
    m_Face[(UINT)FACE_TYPE::eFAR_1] = XMPlaneFromPoints((vWorldPos[5] - vWorldPos[1]) * 0.1f + vWorldPos[1], (vWorldPos[4] - vWorldPos[0]) * 0.1f + vWorldPos[0], (vWorldPos[7] - vWorldPos[3]) * 0.1f + vWorldPos[3]);

    m_Face[(UINT)FACE_TYPE::eNEAR_2] = XMPlaneFromPoints((vWorldPos[4] - vWorldPos[0]) * 0.1f + vWorldPos[0], (vWorldPos[5] - vWorldPos[1]) * 0.1f + vWorldPos[1], (vWorldPos[7] - vWorldPos[3]) * 0.1f + vWorldPos[3]);
    m_Face[(UINT)FACE_TYPE::eFAR_2] = XMPlaneFromPoints(vWorldPos[5], vWorldPos[4], vWorldPos[7]);

    // 0 : 1 ~ 0.01 : 0.99
    Vec3 vCenter = ((vWorldPos[0] + vWorldPos[1] + vWorldPos[2] + vWorldPos[3]) * 1.99f + (vWorldPos[4] + vWorldPos[5] + vWorldPos[6] + vWorldPos[7]) * 0.01f) / 8.f;
    m_CircumscribedSphere[(UINT)CASCADE::eNEAR] = Vec4(vCenter, ((vWorldPos[5] - vWorldPos[1]) * 0.01f + vWorldPos[1] - vCenter).Length());

    // 0.01 : 0.99 ~ 0.1 : 0.9
    vCenter = ((vWorldPos[0] + vWorldPos[1] + vWorldPos[2] + vWorldPos[3]) * 1.89f + (vWorldPos[4] + vWorldPos[5] + vWorldPos[6] + vWorldPos[7]) * 0.11f) / 8.f;
    m_CircumscribedSphere[(UINT)CASCADE::eMIDDLE] = Vec4(vCenter, ((vWorldPos[5] - vWorldPos[1]) * 0.1f + vWorldPos[1] - vCenter).Length());

    // 0.1 : 0.9 ~ 1 : 0
    vCenter = ((vWorldPos[0] + vWorldPos[1] + vWorldPos[2] + vWorldPos[3]) * 0.9f + (vWorldPos[4] + vWorldPos[5] + vWorldPos[6] + vWorldPos[7]) * 1.1f) / 8.f;
    m_CircumscribedSphere[(UINT)CASCADE::eFAR] = Vec4(vCenter, (vWorldPos[5] - vCenter).Length());

    // 0 : 1 ~ 1 : 0
    vCenter = ((vWorldPos[0] + vWorldPos[1] + vWorldPos[2] + vWorldPos[3]) * 1.f + (vWorldPos[4] + vWorldPos[5] + vWorldPos[6] + vWorldPos[7]) * 1.f) / 8.f;
    m_CircumscribedSphere[(UINT)CASCADE::eAll] = Vec4(vCenter, (vWorldPos[5] - vCenter).Length());

}

bool CFrustum::FrustumCheck(Vec3 _WorldPos, CASCADE _Cascade)
{
	for (int i = 0; i < 4; ++i)
	{		
		Vec3 vNormal = m_Face[i];

		if (vNormal.Dot(_WorldPos) + m_Face[i].w > 0)
			return false;
	}

    if (_Cascade != CASCADE::eAll)
    {
        int begin = (UINT)_Cascade * 2 + 4;
        for (int i = begin; i < begin + 2; ++i)
        {
            Vec3 vNormal = m_Face[i];

            if (vNormal.Dot(_WorldPos) + m_Face[i].w > 0)
                return false;
        }
    }
    else
    {
        Vec3 vNormal0 = m_Face[(UINT)FACE_TYPE::eNEAR_0];
        Vec3 vNormal1 = m_Face[(UINT)FACE_TYPE::eFAR_2];

        if (vNormal0.Dot(_WorldPos) + m_Face[(UINT)FACE_TYPE::eNEAR_0].w > 0
            || vNormal1.Dot(_WorldPos) + m_Face[(UINT)FACE_TYPE::eFAR_2].w > 0)
            return false;
    }

	return true;
}

bool CFrustum::FrustumCheckSphere(Vec3 _WorldPos, float _Radius, CASCADE _Cascade)
{
    for (int i = 0; i < 4; ++i)
	{
		Vec3 vNormal = m_Face[i];

		if (vNormal.Dot(_WorldPos) + m_Face[i].w > _Radius)
			return false;
	}

    if (_Cascade != CASCADE::eAll)
    {
        int begin = (UINT)_Cascade * 2 + 4;
        for (int i = begin; i < begin + 2; ++i)
        {
            Vec3 vNormal = m_Face[i];

            if (vNormal.Dot(_WorldPos) + m_Face[i].w > _Radius)
                return false;
        }
    }
    else
    {
        Vec3 vNormal0 = m_Face[(UINT)FACE_TYPE::eNEAR_0];
        Vec3 vNormal1 = m_Face[(UINT)FACE_TYPE::eFAR_2];

        if (vNormal0.Dot(_WorldPos) + m_Face[(UINT)FACE_TYPE::eNEAR_0].w > _Radius
            || vNormal1.Dot(_WorldPos) + m_Face[(UINT)FACE_TYPE::eFAR_2].w > _Radius)
            return false;
    }

	return true;
}