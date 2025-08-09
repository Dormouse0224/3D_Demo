#include "pch.h"
#include "CAnimator3D.h"

#include "CAssetMgr.h"
#include "CTimeMgr.h"
#include "CStructuredBuffer.h"
#include "components.h"

#include "CBoneMatrixCS.h"

CAnimator3D::CAnimator3D()
	: CComponent(COMPONENT_TYPE::ANIMATOR3D)
	, m_vecBones(nullptr)
	, m_vecClip(nullptr)
	, m_CurClip(0)
	, m_CurTime(0.)
	, m_FrameCount(30)
	, m_BoneFinalMatBuffer(nullptr)
	, m_bFinalMatUpdate(false)
	, m_FrameIdx(0)
	, m_NextFrameIdx(0)
	, m_Ratio(0.f)
{
	m_BoneFinalMatBuffer = new CStructuredBuffer;
}

CAnimator3D::CAnimator3D(const CAnimator3D& _origin)
	: m_vecBones(_origin.m_vecBones)
	, m_vecClip(_origin.m_vecClip)
	, m_CurClip(_origin.m_CurClip)
	, m_CurTime(_origin.m_CurTime)
	, m_FrameCount(_origin.m_FrameCount)
	, m_BoneFinalMatBuffer(nullptr)
	, m_bFinalMatUpdate(false)
	, m_FrameIdx(_origin.m_FrameIdx)
	, m_NextFrameIdx(_origin.m_NextFrameIdx)
	, m_Ratio(_origin.m_Ratio)
	, CComponent(COMPONENT_TYPE::ANIMATOR3D)
{
	m_BoneFinalMatBuffer = new CStructuredBuffer;
}

CAnimator3D::~CAnimator3D()
{
	if (nullptr != m_BoneFinalMatBuffer)
		delete m_BoneFinalMatBuffer;
}

void CAnimator3D::FinalTick()
{
    if (RenderComponent() && RenderComponent()->GetMesh().Get())
    {
        SetBones(RenderComponent()->GetMesh()->GetBones());
        SetAnimClip(RenderComponent()->GetMesh()->GetAnimClip());
    }

    if (m_vecClip == nullptr || m_vecClip->size() < 1)
        return;

	m_CurTime = 0.f;
	// ���� ������� Clip �� �ð��� �����Ѵ�.
	m_vecClipUpdateTime[m_CurClip] += EngineDT;

	if (m_vecClipUpdateTime[m_CurClip] >= m_vecClip->at(m_CurClip).dTimeLength)
	{
		m_vecClipUpdateTime[m_CurClip] = 0.f;
	}

	m_CurTime = m_vecClip->at(m_CurClip).dStartTime + m_vecClipUpdateTime[m_CurClip];

	// ���� ������ �ε��� ���ϱ�
	double dFrameIdx = m_CurTime * (double)m_FrameCount;
	m_FrameIdx = (int)(dFrameIdx);

	// ���� ������ �ε���
	if (m_FrameIdx >= m_vecClip->at(0).iFrameLength - 1)
		m_NextFrameIdx = m_FrameIdx;	// ���̸� ���� �ε����� ����
	else
		m_NextFrameIdx = m_FrameIdx + 1;

	// �����Ӱ��� �ð��� ���� ������ �����ش�.
	m_Ratio = (float)(dFrameIdx - (double)m_FrameIdx);

	// ��ǻƮ ���̴� ���꿩��
	m_bFinalMatUpdate = false;
}

void CAnimator3D::SetAnimClip(const vector<tMTAnimClip>& _vecAnimClip)
{
	m_vecClip = &_vecAnimClip;
	m_vecClipUpdateTime.resize(m_vecClip->size());

	// �׽�Ʈ �ڵ�
	/*static float fTime = 0.f;
	fTime += 1.f;
	m_vecClipUpdateTime[0] = fTime;*/
}

void CAnimator3D::SetCurClipIdx(int _Idx)
{
    if (m_vecClip->size() > _Idx)
        m_CurClip = _Idx;
}

void CAnimator3D::Binding(AssetPtr<CMaterial> _Mtrl)
{
    if (m_vecClip == nullptr || m_vecClip->size() < 1)
        return;

	if (!m_bFinalMatUpdate)
	{
        // Animation3D Update Compute Shader
        // ���� �ִϸ��̼� �����ӿ����� ��� ���� ��Ű�� Ʈ�������� ����
		static AssetPtr<CBoneMatrixCS> pBoneMatCS = new CBoneMatrixCS;

		// Bone Data
        AssetPtr<CMesh> pMesh = MeshRender()->GetMesh();
		check_mesh(pMesh);

		pBoneMatCS->SetFrameDataBuffer(pMesh->GetBoneFrameDataBuffer());
		pBoneMatCS->SetOffsetMatBuffer(pMesh->GetBoneInverseBuffer());
		pBoneMatCS->SetOutputBuffer(m_BoneFinalMatBuffer);

		UINT iBoneCount = (UINT)m_vecBones->size();
		pBoneMatCS->SetBoneCount(iBoneCount);
		pBoneMatCS->SetFrameIndex(m_FrameIdx);
		pBoneMatCS->SetNextFrameIdx(m_NextFrameIdx);
		pBoneMatCS->SetFrameRatio(m_Ratio);

		// ������Ʈ ���̴� ����
		pBoneMatCS->Execute();
        
        // IK Compute Shader
        // �����, ����, �� ���� ��ġ�� ����
        // Bip001LThigh/Calf/Foot, Bip001RThigh/Calf/Foot
        LegIKSolution(pMesh->FindBone(L"Bip001LFoot"), m_LeftLegIK);
        LegIKSolution(pMesh->FindBone(L"Bip001RFoot"), m_RightLegIK);

		m_bFinalMatUpdate = true;
	}

	// t17 �������Ϳ� ������� ������(��������) ���ε�		
	m_BoneFinalMatBuffer->Binding(17);

    _Mtrl->SetAnim3D(true); // Animation Mesh �˸���
    _Mtrl->SetBoneCount(Animator3D()->GetBoneCount());
}

void CAnimator3D::ClearData(AssetPtr<CMaterial> _Mtrl)
{
	m_BoneFinalMatBuffer->Unbind();

	UINT iMtrlCount = MeshRender()->GetMaterialCount();
    if (_Mtrl == nullptr)
    {
        AssetPtr<CMaterial> pMtrl = nullptr;
        for (UINT i = 0; i < iMtrlCount; ++i)
        {
            pMtrl = MeshRender()->GetSharedMaterial(i);
            if (nullptr == pMtrl)
                continue;

            pMtrl->SetAnim3D(false); // Animation Mesh �˸���
            pMtrl->SetBoneCount(0);
        }
    }
    else
    {
        _Mtrl->SetAnim3D(false); // Animation Mesh �˸���
        _Mtrl->SetBoneCount(0);
    }
}

void CAnimator3D::check_mesh(AssetPtr<CMesh> _pMesh)
{
	UINT iBoneCount = _pMesh->GetBoneCount();
	if (m_BoneFinalMatBuffer->GetElementCount() != iBoneCount)
	{
		m_BoneFinalMatBuffer->Create(sizeof(Matrix), iBoneCount, SB_TYPE::SRV_UAV, true, nullptr);
	}
}

void CAnimator3D::LegIKSolution(const tMTBone* _pBoneFoot, Vec3 _Offset)
{
    if (_pBoneFoot == nullptr)
        return;
    if (_Offset == Vec3(0, 0, 0))
        return;

    AssetPtr<CMesh> pMesh = MeshRender()->GetMesh();
    const tMTBone* pBoneCalf = pMesh->GetBone(_pBoneFoot->iParentIndx);
    if (pBoneCalf == nullptr) return;
    const tMTBone* pBoneThigh = pMesh->GetBone(pBoneCalf->iParentIndx);
    if (pBoneThigh == nullptr) return;

    vector<Matrix> vecInvMat(pMesh->GetBoneInverseBuffer()->GetElementCount());
    pMesh->GetBoneInverseBuffer()->GetData(vecInvMat.data());
    vector<Matrix> vecFinMat(m_BoneFinalMatBuffer->GetElementCount());
    m_BoneFinalMatBuffer->GetData(vecFinMat.data());
    if (vecFinMat.size() < _pBoneFoot->iIdx || vecFinMat.size() < pBoneCalf->iIdx || vecFinMat.size() < pBoneThigh->iIdx) return;
    
    // IK ���� �� ���� ���� ��ȯ��İ� ��ǥ
    Matrix matFoot = _pBoneFoot->matOffset.Invert() * vecFinMat[_pBoneFoot->iIdx].Transpose();
    Matrix matCalf = pBoneCalf->matOffset.Invert() * vecFinMat[pBoneCalf->iIdx].Transpose();
    Matrix matThigh = pBoneThigh->matOffset.Invert() * vecFinMat[pBoneThigh->iIdx].Transpose();
    Vec3 posFootOrigin = matFoot.Translation();
    Vec3 posCalfOrigin = matCalf.Translation();
    Vec3 posThigh = matThigh.Translation();

    // �����-���Ƹ� �� ���ݰ� ���Ƹ�-�� �� ����
    float lenTC = (posCalfOrigin - posThigh).Length();
    float lenCF = (posFootOrigin - posCalfOrigin).Length();

    // IK ���� ��ǥ ��ġ
    Vec3 posFootTarget = posFootOrigin + _Offset;
    float lenTF = (posFootTarget - posThigh).Length();
    if (lenTF > lenTC + lenCF) return;
    float d = (lenTC * lenTC - lenCF * lenCF + lenTF * lenTF) / (2.f * lenTF);
    float h = sqrt(lenTC * lenTC - d * d);
    Vec3 vTFTarget = (posFootTarget - posThigh).Normalize();
    Vec3 vRight = XMVector3TransformNormal(Vec3(0, 1, 0), matCalf);
    Vec3 vPole = vTFTarget.Cross(vRight).Normalize();
    Vec3 posCalfTarget = posThigh + d * vTFTarget + h * vPole;

    Vec3 vTCOrigin = posCalfOrigin - posThigh;
    Vec3 vTCTarget = posCalfTarget - posThigh;
    Matrix matRotT = XMMatrixRotationQuaternion(XMQuaternionRotationVectorToVector(vTCOrigin, vTCTarget));
    vecFinMat = RotateBone(pBoneThigh, matRotT, vecFinMat);
    Vec3 vCFOrigin = posFootOrigin - posCalfOrigin;
    Vec3 vCFTarget = posFootTarget - posCalfTarget;
    vCFOrigin = XMVector3TransformNormal(vCFOrigin, matRotT);
    Matrix matRotC = XMMatrixRotationQuaternion(XMQuaternionRotationVectorToVector(vCFOrigin, vCFTarget));
    vecFinMat = RotateBone(pBoneCalf, matRotC, vecFinMat);

    Vec3 posCalf2 = (pBoneCalf->matOffset.Invert() * vecFinMat[pBoneCalf->iIdx].Transpose()).Translation();
    Vec3 posFoot2 = (_pBoneFoot->matOffset.Invert() * vecFinMat[_pBoneFoot->iIdx].Transpose()).Translation();
    m_BoneFinalMatBuffer->SetData(vecFinMat.data());
}

vector<Matrix> CAnimator3D::RotateBone(const tMTBone* _pBone, Matrix _RotMat, const vector<Matrix>& _vecMat)
{
    AssetPtr<CMesh> pMesh = MeshRender()->GetMesh();
    vector<Matrix> vecFinMat = _vecMat;
    int count = 0;
    // �Է¹��� ���� ȸ����Ų��
    Matrix matBone = _pBone->matOffset.Invert() * _vecMat[_pBone->iIdx].Transpose();
    Vec3 vTrans = matBone.Translation();
    matBone._41 -= vTrans.x; matBone._42 -= vTrans.y; matBone._43 -= vTrans.z;
    matBone *= _RotMat;
    matBone._41 += vTrans.x; matBone._42 += vTrans.y; matBone._43 += vTrans.z;
    vecFinMat[_pBone->iIdx] = (_pBone->matOffset * matBone).Transpose();

    // ȸ���� ���� �ڽ� ������ ���� Ʈ�������� �̿��� �����Ѵ�
    queue<int> q;
    for (int i : _pBone->vecChildIdx)
    {
        ++count;
        q.push(i);
    }
    while (!q.empty())
    {
        const tMTBone* pChildBone = pMesh->GetBone(q.front());
        const tMTBone* pParentBone = pMesh->GetBone(pChildBone->iParentIndx);
        q.pop();
        if (pChildBone->vecChildIdx.empty())
            continue;
        for (int i : pChildBone->vecChildIdx)
        {
            ++count;
            q.push(i);
        }

        Matrix matChBone = pChildBone->matOffset.Invert() * _vecMat[pChildBone->iIdx].Transpose();
        //Vec3 vTrans = matChBone.Translation();
        matChBone._41 -= vTrans.x; matChBone._42 -= vTrans.y; matChBone._43 -= vTrans.z;
        matChBone *= _RotMat;
        matChBone._41 += vTrans.x; matChBone._42 += vTrans.y; matChBone._43 += vTrans.z;
        vecFinMat[pChildBone->iIdx] = (pChildBone->matOffset * matChBone).Transpose();

        //Matrix matChildBone = pChildBone->matOffset.Invert() * _vecMat[pChildBone->iIdx].Transpose();
        //Matrix matParentBone = pParentBone->matOffset.Invert() * _vecMat[pParentBone->iIdx].Transpose();
        //Matrix locMat = matParentBone.Invert() * matChildBone;
        //Matrix matNewParentBone = pParentBone->matOffset.Invert() * vecFinMat[pParentBone->iIdx].Transpose();
        //Matrix matNewThisBone = matNewParentBone * locMat;
        //vecFinMat[pChildBone->iIdx] = (pChildBone->matOffset * matNewThisBone).Transpose();

        //Matrix locMat = _vecMat[pChildBone->iParentIndx].Transpose().Invert() * _vecMat[pChildBone->iIdx].Transpose();
        //vecFinMat[pChildBone->iIdx] = (vecFinMat[pChildBone->iParentIndx].Transpose() * locMat).Transpose();
    }
    int a = 0;
    return vecFinMat;
}

int CAnimator3D::Save(fstream& _Stream)
{
    return S_OK;
}

int CAnimator3D::Load(fstream& _Stream)
{
    return S_OK;
}