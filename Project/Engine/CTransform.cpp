#include "pch.h"
#include "CTransform.h"

#include "CDevice.h"
#include "CConstBuffer.h"
#include "CPhysxActor.h"


CTransform::CTransform()
	: CComponent(COMPONENT_TYPE::TRANSFORM)
	, m_RelativePos(Vec3(0.f, 0.f, 0.f))
	, m_RelativeScale(Vec3(1.f, 1.f, 1.f))
	, m_RelativeRot(Vec4(0.f, 0.f, 0.f, 1.f))
	, m_RelativeDir{}
	, m_WorldDir{}
	, m_matWorld()
	, m_IndependentScale(false)
{
	m_RelativeDir[(UINT)DIR::RIGHT] = Vec3(1.f, 0.f, 0.f);
	m_RelativeDir[(UINT)DIR::UP] = Vec3(0.f, 1.f, 0.f);
	m_RelativeDir[(UINT)DIR::FRONT] = Vec3(0.f, 0.f, 1.f);
}

CTransform::~CTransform()
{
}

void CTransform::FinalTick()
{
	CalcMatWorld();

	//// 부모 오브젝트가 있는 경우, 부모의 월드행렬을 누적시킨다.
	//CGameObject* pParent = GetOwner()->GetParent();
	//if (pParent)
	//{
	//	if (!m_IndependentScale)
	//	{
	//		Vec3 vParentScale = pParent->Transform()->GetWorldScale();
	//		m_matWorld *= XMMatrixScaling(vParentScale.x, vParentScale.y, vParentScale.z);
	//	}
	//	if (!m_IndependentRot)
	//	{
	//		Vec3 vParentRot = pParent->Transform()->GetWorldRot();
	//		m_matWorld *= XMMatrixRotationQuaternion(vParentRot);
	//	}
	//	if (!m_IndependentTrans)
	//	{
	//		Vec3 vParentTrans = pParent->Transform()->GetWorldTrans();
	//		m_matWorld *= XMMatrixTranslation(vParentTrans.x, vParentTrans.y, vParentTrans.z);
	//	}

	//	m_WorldDir[(UINT)DIR::RIGHT] = Vec3(1.f, 0.f, 0.f);
	//	m_WorldDir[(UINT)DIR::UP] = Vec3(0.f, 1.f, 0.f);
	//	m_WorldDir[(UINT)DIR::FRONT] = Vec3(0.f, 0.f, 1.f);

	//	for (int i = 0; i < 3; ++i)
	//	{
	//		XMVECTOR vScale;
	//		XMVECTOR vRot;
	//		XMVECTOR vTrans;
	//		XMMatrixDecompose(&vScale, &vRot, &vTrans, m_matWorld);
	//		Vec3 Scale = vScale;
	//		Matrix matScaleInv = XMMatrixInverse(nullptr, XMMatrixScaling(Scale.x, Scale.y, Scale.z));

	//		m_WorldDir[i] = XMVector3TransformNormal(m_WorldDir[i], matScaleInv * m_matWorld);
	//		m_WorldDir[i].Normalize();
	//	}
	//}
}

// SIMD

void CTransform::Binding()
{
	// 오브젝트의 위치를 상수버퍼로 이동시킨다.
	CConstBuffer* pTransformBuffer = CDevice::GetInst()->GetConstBuffer(CB_TYPE::TRANSFORM);

	g_Trans.matWorld = m_matWorld;
	g_Trans.matWV = g_Trans.matWorld * g_Trans.matView;
	g_Trans.matVP = g_Trans.matView * g_Trans.matProj;
	g_Trans.matWVP = g_Trans.matWV * g_Trans.matProj;

	pTransformBuffer->SetData(&g_Trans, sizeof(tTransform));
	pTransformBuffer->Binding();
}

void CTransform::SetRelativePos(Vec3 _Pos)
{
	m_RelativePos = _Pos;
	if (PhysxActor())
	{
		CalcMatWorld();
		PhysxActor()->UpdatePosition(GetWorldTrans());
	}
}

Vec3 CTransform::GetWorldTrans()
{
	Vec3 vWorldTrans = m_RelativePos;

	CGameObject* pParent = GetOwner()->GetParent();
	bool bIndependent = m_IndependentTrans;

	while (pParent && !bIndependent)
	{
		vWorldTrans += pParent->Transform()->GetRelativePos();

		bIndependent = pParent->Transform()->m_IndependentTrans;
		pParent = pParent->GetParent();
	}

	return vWorldTrans;
}

Vec3 CTransform::GetWorldScale()
{
	Vec3 vWorldScale = m_RelativeScale;

	CGameObject* pParent = GetOwner()->GetParent();
	bool bIndependent = m_IndependentScale;

	while (pParent && !bIndependent)
	{
		vWorldScale *= pParent->Transform()->GetRelativeScale();

		bIndependent = pParent->Transform()->m_IndependentScale;
		pParent = pParent->GetParent();
	}

	return vWorldScale;
}

Vec4 CTransform::GetWorldRot()
{
	Vec4 vWorldRot = m_RelativeRot;

	CGameObject* pParent = GetOwner()->GetParent();
	bool bIndependent = m_IndependentRot;

	while (pParent && !bIndependent)
	{
		// (누적된 쿼터니언) * (부모의 쿼터니언)
		vWorldRot = XMQuaternionMultiply(pParent->Transform()->GetRelativeRotQuat(), vWorldRot);

		bIndependent = pParent->Transform()->m_IndependentRot;
		pParent = pParent->GetParent();
	}

	return vWorldRot;
}

Vec3 CTransform::GetRelativeRotEuler()
{
	/*float rad = acos(m_RelativeRot.w) * 2;
	float pitch = asin(2 * (m_RelativeRot.w * m_RelativeRot.y - m_RelativeRot.x * m_RelativeRot.z));
	float yaw = atan2(2 * (m_RelativeRot.w * m_RelativeRot.x + m_RelativeRot.y * m_RelativeRot.z), 1 - 2 * (pow(m_RelativeRot.y, 2) + pow(m_RelativeRot.z, 2)));
	float roll = atan2(2 * (m_RelativeRot.w * m_RelativeRot.z + m_RelativeRot.x * m_RelativeRot.y), 1 - 2 * (pow(m_RelativeRot.z, 2) + pow(m_RelativeRot.x, 2)));*/
	
	const Vec4& q = m_RelativeRot;

	// ZYX 회전 순서 (Roll → Yaw → Pitch)
	float sinr_cosp = 2.0f * (q.w * q.x + q.y * q.z);
	float cosr_cosp = 1.0f - 2.0f * (q.x * q.x + q.y * q.y);
	float pitch = atan2(sinr_cosp, cosr_cosp); // X축 회전

	float sinp = 2.0f * (q.w * q.y - q.z * q.x);
	float yaw;
	if (abs(sinp) >= 1.0f)
		yaw = copysign(XM_PI / 2.0f, sinp); // clamp to 90 deg if out of range
	else
		yaw = asin(sinp); // Y축 회전

	float siny_cosp = 2.0f * (q.w * q.z + q.x * q.y);
	float cosy_cosp = 1.0f - 2.0f * (q.y * q.y + q.z * q.z);
	float roll = atan2(siny_cosp, cosy_cosp); // Z축 회전

	Vec3 vRotation(pitch * 180 / XM_PI, yaw * 180 / XM_PI, roll * 180 / XM_PI);
	return vRotation;
}

int CTransform::Load(fstream& _Stream)
{
	if (!_Stream.is_open())
		return E_FAIL;

	// 크기, 회전, 위치 정보와 크기 독립성 여부 불러오기
	_Stream.read(reinterpret_cast<char*>(&m_RelativePos), sizeof(Vec3));
	_Stream.read(reinterpret_cast<char*>(&m_RelativeScale), sizeof(Vec3));
	_Stream.read(reinterpret_cast<char*>(&m_RelativeRot), sizeof(Vec4));
	_Stream.read(reinterpret_cast<char*>(&m_IndependentScale), sizeof(bool));
	_Stream.read(reinterpret_cast<char*>(&m_IndependentRot), sizeof(bool));
	_Stream.read(reinterpret_cast<char*>(&m_IndependentTrans), sizeof(bool));

	return S_OK;
}

int CTransform::Save(fstream& _Stream)
{
	// m_RelativeDir, m_WorldDir, m_matWorld 는 매 틱마다 계산되므로 저장하지 않음

	if (!_Stream.is_open())
		return E_FAIL;

	// 크기, 회전, 위치 정보와 크기 독립성 여부 저장
	_Stream.write(reinterpret_cast<char*>(&m_RelativePos), sizeof(Vec3));
	_Stream.write(reinterpret_cast<char*>(&m_RelativeScale), sizeof(Vec3));
	_Stream.write(reinterpret_cast<char*>(&m_RelativeRot), sizeof(Vec4));
	_Stream.write(reinterpret_cast<char*>(&m_IndependentScale), sizeof(bool));
	_Stream.write(reinterpret_cast<char*>(&m_IndependentRot), sizeof(bool));
	_Stream.write(reinterpret_cast<char*>(&m_IndependentTrans), sizeof(bool));


	return S_OK;
}

void CTransform::CalcMatWorld()
{
	// 위치, 크기, 회전 정보를 월드행렬로 변환
	// Scale
	Vec3 worldScale = GetWorldScale();
	Matrix matScale = XMMatrixScaling(worldScale.x, worldScale.y, worldScale.z);
	//Matrix matScale = XMMatrixScaling(m_RelativeScale.x, m_RelativeScale.y, m_RelativeScale.z);

	// Rotation
	Matrix matRot = XMMatrixRotationQuaternion(GetWorldRot());
	//Matrix matRot = XMMatrixRotationQuaternion(m_RelativeRot);

	// Translation
	Vec3 worldTrans = GetWorldTrans();
	Matrix matTrans = XMMatrixTranslation(worldTrans.x, worldTrans.y, worldTrans.z);
	//Matrix matTrans = XMMatrixTranslation(m_RelativePos.x, m_RelativePos.y, m_RelativePos.z);

	// 크기 회전 이동 부모 순서로 적용
	m_matWorld = matScale * matRot * matTrans;

	// 오브젝트의 방향정보 계산
	m_RelativeDir[(UINT)DIR::RIGHT] = Vec3(1.f, 0.f, 0.f);
	m_RelativeDir[(UINT)DIR::UP] = Vec3(0.f, 1.f, 0.f);
	m_RelativeDir[(UINT)DIR::FRONT] = Vec3(0.f, 0.f, 1.f);

	// 방향벡터에 회전행렬을 적용해서 현재 방향값을 계산한다.
	// XMVector3TransformCoord()  w 동차좌표 값을 1로 확장
	// XMVector3TransformNormal() w 동차좌표값을 0으로 확장
	for (int i = 0; i < (int)DIR::END; ++i)
	{
		m_WorldDir[i] = m_RelativeDir[i] = XMVector3TransformNormal(m_RelativeDir[i], matRot);
	}
}

void CTransform::SetRelativeRot(Vec4 _RotationQuat)
{
	m_RelativeRot = _RotationQuat;
	if (PhysxActor())
		PhysxActor()->UpdateRotation(m_RelativeRot);
}

void CTransform::SetRelativeRot(Vec3 _RotationDeg)
{
	Vec3 rad = _RotationDeg / 180.f * XM_PI;
	Vec4 vQuatX = XMQuaternionRotationAxis(Vec3(1, 0, 0), rad.x);
	Vec4 vQuatY = XMQuaternionRotationAxis(Vec3(0, 1, 0), rad.y);
	Vec4 vQuatZ = XMQuaternionRotationAxis(Vec3(0, 0, 1), rad.z);
	m_RelativeRot = XMQuaternionMultiply(vQuatX, XMQuaternionMultiply(vQuatY, vQuatZ));

	// 강체 보유 시 피직스에 회전정보 적용
	if (PhysxActor())
		PhysxActor()->UpdateRotation(m_RelativeRot);
}

void CTransform::SetRelativeRot(float _xDeg, float _yDeg, float _zDeg)
{
	SetRelativeRot(Vec3(_xDeg, _yDeg, _zDeg));
}
