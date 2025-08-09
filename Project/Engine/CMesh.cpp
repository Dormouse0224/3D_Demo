#include "pch.h"
#include "CMesh.h"

#include "CDevice.h"
#include "CPathMgr.h"
#include "CAssetMgr.h"
#include "CRenderMgr.h"

CMesh::CMesh()
	: CAsset(ASSET_TYPE::MESH)
	, m_VBDesc{}
    , m_VtxCount(0)
	, m_VtxSysMem(nullptr)
{
}

CMesh::~CMesh()
{
	if (nullptr != m_VtxSysMem)
		delete m_VtxSysMem;

    for (size_t i = 0; i < m_vecIdxInfo.size(); ++i)
    {
        if (nullptr != m_vecIdxInfo[i].IdxSysMem)
            delete m_vecIdxInfo[i].IdxSysMem;
    }

    if (nullptr != m_BoneFrameData)
        delete m_BoneFrameData;

    if (nullptr != m_BoneInverse)
        delete m_BoneInverse;
}

int CMesh::Create(Vtx* _VtxSysMem, UINT _VtxCount, UINT* _IdxSysMem, UINT _IdxCount)
{
	m_VtxCount = _VtxCount;

	// 정점 버퍼 생성
	m_VBDesc.ByteWidth = sizeof(Vtx) * _VtxCount;

	// 생성 이후엔 데이터 이동 없음
	m_VBDesc.Usage = D3D11_USAGE_DEFAULT;
	m_VBDesc.CPUAccessFlags = 0;
	m_VBDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	D3D11_SUBRESOURCE_DATA Sub = {};
	Sub.pSysMem = _VtxSysMem;

	DEVICE->CreateBuffer(&m_VBDesc, &Sub, m_VB.GetAddressOf());


    // 인덱스 버퍼 생성
    tIndexInfo IndexInfo = {};
    IndexInfo.IdxCount = _IdxCount;

    IndexInfo.tIBDesc.ByteWidth = sizeof(UINT) * _IdxCount;

    // 버퍼 생성 이후에도, 버퍼의 내용을 수정 할 수 있는 옵션
    IndexInfo.tIBDesc.CPUAccessFlags = 0;
    IndexInfo.tIBDesc.Usage = D3D11_USAGE::D3D11_USAGE_DEFAULT;

    // 정점을 저장하는 목적의 버퍼 임을 알림
    IndexInfo.tIBDesc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_INDEX_BUFFER;
    IndexInfo.tIBDesc.MiscFlags = 0;
    IndexInfo.tIBDesc.StructureByteStride = 0;

    // 초기 데이터를 넘겨주기 위한 정보 구조체	
    Sub.pSysMem = _IdxSysMem;

    if (FAILED(DEVICE->CreateBuffer(&IndexInfo.tIBDesc, &Sub, IndexInfo.IB.GetAddressOf())))
    {
        assert(nullptr);
    }

    // System 메모리에 정점과 인덱스 정보 기록
    m_VtxSysMem = new Vtx[m_VtxCount];
    memcpy(m_VtxSysMem, _VtxSysMem, sizeof(Vtx) * m_VtxCount);

    IndexInfo.IdxSysMem = new UINT[IndexInfo.IdxCount];
    memcpy(IndexInfo.IdxSysMem, _IdxSysMem, sizeof(UINT) * IndexInfo.IdxCount);
    m_vecIdxInfo.push_back(IndexInfo);

	DEBUG_NAME(m_VB, "CMesh_VB");

	return S_OK;
}

CMesh* CMesh::CreateFromContainer(CFBXLoader& _loader)
{
    const tContainer* container = &_loader.GetContainer(0);

    UINT iVtxCount = (UINT)container->vecPos.size();

    D3D11_BUFFER_DESC tVtxDesc = {};

    tVtxDesc.ByteWidth = sizeof(Vtx) * iVtxCount;
    tVtxDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

    tVtxDesc.Usage = D3D11_USAGE_DEFAULT;
    if (D3D11_USAGE_DYNAMIC == tVtxDesc.Usage)
        tVtxDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

    D3D11_SUBRESOURCE_DATA tSub = {};
    tSub.pSysMem = malloc(tVtxDesc.ByteWidth);
    Vtx* pSys = (Vtx*)tSub.pSysMem;
    for (UINT i = 0; i < iVtxCount; ++i)
    {
        pSys[i].vPos = container->vecPos[i];
        pSys[i].vUV = container->vecUV[i];
        pSys[i].vColor = Vec4(1.f, 0.f, 1.f, 1.f);
        pSys[i].vNormal = container->vecNormal[i];
        pSys[i].vTangent = container->vecTangent[i];
        pSys[i].vBinormal = container->vecBinormal[i];
        pSys[i].vWeights = container->vecWeights[i];
        pSys[i].vIndices = container->vecIndices[i];
    }

    ComPtr<ID3D11Buffer> pVB = NULL;
    if (FAILED(DEVICE->CreateBuffer(&tVtxDesc, &tSub, pVB.GetAddressOf())))
    {
        return NULL;
    }

    CMesh* pMesh = new CMesh;
    pMesh->m_VB = pVB;
    pMesh->m_VBDesc = tVtxDesc;
    pMesh->m_VtxCount = iVtxCount;
    pMesh->m_VtxSysMem = pSys;

    // 인덱스 정보
    UINT iIdxBufferCount = (UINT)container->vecIdx.size();
    D3D11_BUFFER_DESC tIdxDesc = {};

    for (UINT i = 0; i < iIdxBufferCount; ++i)
    {
        tIdxDesc.ByteWidth = (UINT)container->vecIdx[i].size() * sizeof(UINT); // Index Format 이 R32_UINT 이기 때문
        tIdxDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
        tIdxDesc.Usage = D3D11_USAGE_DEFAULT;
        if (D3D11_USAGE_DYNAMIC == tIdxDesc.Usage)
            tIdxDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

        void* pSysMem = malloc(tIdxDesc.ByteWidth);
        memcpy(pSysMem, container->vecIdx[i].data(), tIdxDesc.ByteWidth);
        tSub.pSysMem = pSysMem;

        ComPtr<ID3D11Buffer> pIB = nullptr;
        if (FAILED(DEVICE->CreateBuffer(&tIdxDesc, &tSub, pIB.GetAddressOf())))
        {
            return NULL;
        }

        tIndexInfo info = {};
        info.tIBDesc = tIdxDesc;
        info.IdxCount = (UINT)container->vecIdx[i].size();
        info.IdxSysMem = pSysMem;
        info.IB = pIB;

        pMesh->m_vecIdxInfo.push_back(info);
    }

    // Animation3D
    if (!container->bAnimation)
        return pMesh;

    vector<tBone*>& vecBone = _loader.GetBones();
    UINT iFrameCount = 0;
    for (UINT i = 0; i < vecBone.size(); ++i)
    {
        tMTBone bone = {};
        bone.iDepth = vecBone[i]->iDepth;
        bone.iIdx = vecBone[i]->iIdx;
        bone.iParentIndx = vecBone[i]->iParentIndx;
        bone.matBone = GetMatrixFromFbxMatrix(vecBone[i]->matBone);
        bone.matOffset = GetMatrixFromFbxMatrix(vecBone[i]->matOffset);
        bone.strBoneName = vecBone[i]->strBoneName;
        bone.vecChildIdx = vecBone[i]->vecChildIdx;

        for (UINT j = 0; j < vecBone[i]->vecKeyFrame.size(); ++j)
        {
            tMTKeyFrame tKeyframe = {};
            tKeyframe.dTime = vecBone[i]->vecKeyFrame[j].dTime;
            tKeyframe.iFrame = j;
            tKeyframe.vTranslate.x = (float)vecBone[i]->vecKeyFrame[j].matTransform.GetT().mData[0];
            tKeyframe.vTranslate.y = (float)vecBone[i]->vecKeyFrame[j].matTransform.GetT().mData[1];
            tKeyframe.vTranslate.z = (float)vecBone[i]->vecKeyFrame[j].matTransform.GetT().mData[2];

            tKeyframe.vScale.x = (float)vecBone[i]->vecKeyFrame[j].matTransform.GetS().mData[0];
            tKeyframe.vScale.y = (float)vecBone[i]->vecKeyFrame[j].matTransform.GetS().mData[1];
            tKeyframe.vScale.z = (float)vecBone[i]->vecKeyFrame[j].matTransform.GetS().mData[2];

            tKeyframe.qRot.x = (float)vecBone[i]->vecKeyFrame[j].matTransform.GetQ().mData[0];
            tKeyframe.qRot.y = (float)vecBone[i]->vecKeyFrame[j].matTransform.GetQ().mData[1];
            tKeyframe.qRot.z = (float)vecBone[i]->vecKeyFrame[j].matTransform.GetQ().mData[2];
            tKeyframe.qRot.w = (float)vecBone[i]->vecKeyFrame[j].matTransform.GetQ().mData[3];

            bone.vecKeyFrame.push_back(tKeyframe);
        }

        iFrameCount = max(iFrameCount, (UINT)bone.vecKeyFrame.size());

        pMesh->m_vecBones.push_back(bone);
    }

    vector<tAnimClip*>& vecAnimClip = _loader.GetAnimClip();

    for (UINT i = 0; i < vecAnimClip.size(); ++i)
    {
        tMTAnimClip tClip = {};

        tClip.strAnimName = vecAnimClip[i]->strName;
        tClip.dStartTime = vecAnimClip[i]->tStartTime.GetSecondDouble();
        tClip.dEndTime = vecAnimClip[i]->tEndTime.GetSecondDouble();
        tClip.dTimeLength = tClip.dEndTime - tClip.dStartTime;

        tClip.iStartFrame = (int)vecAnimClip[i]->tStartTime.GetFrameCount(vecAnimClip[i]->eMode);
        tClip.iEndFrame = (int)vecAnimClip[i]->tEndTime.GetFrameCount(vecAnimClip[i]->eMode);
        tClip.iFrameLength = tClip.iEndFrame - tClip.iStartFrame;
        tClip.eMode = vecAnimClip[i]->eMode;

        pMesh->m_vecAnimClip.push_back(tClip);
    }

    // Animation 이 있는 Mesh 경우 structuredbuffer 만들어두기
    if (pMesh->IsAnimMesh())
    {
        // BoneOffet 행렬
        vector<Matrix> vecOffset;
        vector<tFrameTrans> vecFrameTrans;
        vecFrameTrans.resize((UINT)pMesh->m_vecBones.size() * iFrameCount);

        for (size_t i = 0; i < pMesh->m_vecBones.size(); ++i)
        {
            vecOffset.push_back(pMesh->m_vecBones[i].matOffset);

            for (size_t j = 0; j < pMesh->m_vecBones[i].vecKeyFrame.size(); ++j)
            {
                vecFrameTrans[(UINT)pMesh->m_vecBones.size() * j + i]
                    = tFrameTrans{ Vec4(pMesh->m_vecBones[i].vecKeyFrame[j].vTranslate, 0.f)
                    , Vec4(pMesh->m_vecBones[i].vecKeyFrame[j].vScale, 0.f)
                    , pMesh->m_vecBones[i].vecKeyFrame[j].qRot };
            }
        }

        pMesh->m_BoneInverse = new CStructuredBuffer;
        pMesh->m_BoneInverse->Create(sizeof(Matrix), (UINT)vecOffset.size(), SB_TYPE::SRV_ONLY, true, vecOffset.data());

        pMesh->m_BoneFrameData = new CStructuredBuffer;
        pMesh->m_BoneFrameData->Create(sizeof(tFrameTrans), (UINT)vecOffset.size() * iFrameCount
            , SB_TYPE::SRV_ONLY, false, vecFrameTrans.data());
    }

    return pMesh;
}


void CMesh::Binding(UINT _iSubset)
{
	UINT Stride = sizeof(Vtx);
	UINT Offset = 0;
	CONTEXT->IASetVertexBuffers(0, 1, m_VB.GetAddressOf(), &Stride, &Offset);
    CONTEXT->IASetIndexBuffer(m_vecIdxInfo[_iSubset].IB.Get(), DXGI_FORMAT_R32_UINT, 0);
}

const tMTBone* CMesh::FindBone(wstring _Name)
{
    for (const tMTBone& bone : m_vecBones)
    {
        if (bone.strBoneName == _Name)
            return &bone;
    }

    return nullptr;
}

void CMesh::Render(UINT _iSubset)
{
	Binding(_iSubset);

    CONTEXT->DrawIndexed(m_vecIdxInfo[_iSubset].IdxCount, 0, 0);
}

void CMesh::Render_Instanced(UINT _Count, UINT _iSubset)
{
    // 0 번 인덱스버퍼를 사용하는것으로 가정
	Binding(_iSubset);

    CONTEXT->DrawIndexedInstanced(m_vecIdxInfo[_iSubset].IdxCount, _Count, 0, 0, 0);
}

int CMesh::Save(const wstring& _FileName, bool _Update)
{
	std::filesystem::path path = CPathMgr::GetContentDir() + std::wstring(L"Mesh\\") + _FileName + std::wstring(L".mesh");
	CPathMgr::CreateParentDir(path);
	std::fstream file(path, std::ios::out | std::ios::binary);

	// 정점 버퍼 정보 저장
	file.write(reinterpret_cast<char*>(&m_VBDesc), sizeof(D3D11_BUFFER_DESC));
	file.write(reinterpret_cast<char*>(&m_VtxCount), sizeof(UINT));
	if (m_VtxCount != 0)
		file.write(reinterpret_cast<char*>(m_VtxSysMem), sizeof(Vtx) * m_VtxCount);

	// 인덱스 버퍼 정보 저장
    int indexCount = m_vecIdxInfo.size();
    file.write(reinterpret_cast<char*>(&indexCount), sizeof(int));
    for (int i = 0; i < indexCount; ++i)
    {
        file.write(reinterpret_cast<char*>(&m_vecIdxInfo[i]), sizeof(tIndexInfo));
        file.write(reinterpret_cast<char*>(m_vecIdxInfo[i].IdxSysMem), m_vecIdxInfo[i].IdxCount * sizeof(UINT));
    }

    // Animation3D 정보 
    UINT iCount = (UINT)m_vecAnimClip.size();
    file.write(reinterpret_cast<char*>(&iCount), sizeof(UINT));
    for (UINT i = 0; i < iCount; ++i)
    {
        SaveWString(m_vecAnimClip[i].strAnimName, file);
        file.write(reinterpret_cast<char*>(&m_vecAnimClip[i].dStartTime), sizeof(double));
        file.write(reinterpret_cast<char*>(&m_vecAnimClip[i].dEndTime), sizeof(double));
        file.write(reinterpret_cast<char*>(&m_vecAnimClip[i].dTimeLength), sizeof(double));
        file.write(reinterpret_cast<char*>(&m_vecAnimClip[i].eMode), sizeof(int));
        file.write(reinterpret_cast<char*>(&m_vecAnimClip[i].fUpdateTime), sizeof(float));
        file.write(reinterpret_cast<char*>(&m_vecAnimClip[i].iStartFrame), sizeof(int));
        file.write(reinterpret_cast<char*>(&m_vecAnimClip[i].iEndFrame), sizeof(int));
        file.write(reinterpret_cast<char*>(&m_vecAnimClip[i].iFrameLength), sizeof(int));
    }

    iCount = (UINT)m_vecBones.size();
    file.write(reinterpret_cast<char*>(&iCount), sizeof(UINT));

    for (UINT i = 0; i < iCount; ++i)
    {
        SaveWString(m_vecBones[i].strBoneName, file);
        file.write(reinterpret_cast<char*>(&m_vecBones[i].iDepth), sizeof(int));
        file.write(reinterpret_cast<char*>(&m_vecBones[i].iIdx), sizeof(int));
        file.write(reinterpret_cast<char*>(&m_vecBones[i].iParentIndx), sizeof(int));
        file.write(reinterpret_cast<char*>(&m_vecBones[i].matBone), sizeof(Matrix));
        file.write(reinterpret_cast<char*>(&m_vecBones[i].matOffset), sizeof(Matrix));

        int iFrameCount = (int)m_vecBones[i].vecKeyFrame.size();
        file.write(reinterpret_cast<char*>(&iFrameCount), sizeof(int));
        for (int j = 0; j < m_vecBones[i].vecKeyFrame.size(); ++j)
        {
            file.write(reinterpret_cast<char*>(&m_vecBones[i].vecKeyFrame[j]), sizeof(tMTKeyFrame));
        }

        int iChildCount = (int)m_vecBones[i].vecChildIdx.size();
        file.write(reinterpret_cast<char*>(&iChildCount), sizeof(int));
        for (int j = 0; j < m_vecBones[i].vecChildIdx.size(); ++j)
        {
            file.write(reinterpret_cast<char*>(&m_vecBones[i].vecChildIdx[j]), sizeof(int));
        }
    }

	file.close();
	return S_OK;
}

int CMesh::Load(const wstring& _FilePath)
{
	std::fstream file(CPathMgr::GetContentDir() + _FilePath, std::ios::in | std::ios::binary);

	if (!file.is_open())
	{
		MessageBoxW(nullptr, L"파일을 찾을 수 없습니다.\n경로를 확인해 주세요.", L"Mesh Load Error", MB_OK);
		return E_FAIL;
	}

	// 정점 버퍼 정보 불러오기
	file.read((char*)&m_VBDesc, sizeof(D3D11_BUFFER_DESC));
	file.read((char*)&m_VtxCount, sizeof(UINT));
	if (m_VtxCount != 0)
	{
		delete[] m_VtxSysMem;
		m_VtxSysMem = new Vtx[m_VtxCount];
		file.read((char*)m_VtxSysMem, sizeof(Vtx) * m_VtxCount);
	}

    // 정점 버퍼 생성
    D3D11_SUBRESOURCE_DATA Sub = {};
    Sub.pSysMem = m_VtxSysMem;
    DEVICE->CreateBuffer(&m_VBDesc, &Sub, m_VB.GetAddressOf());

	// 인덱스 버퍼 정보 불러오기
    int indexCount = 0;
    file.read(reinterpret_cast<char*>(&indexCount), sizeof(int));
    for (int i = 0; i < indexCount; ++i)
    {
        tIndexInfo info = {};
        file.read(reinterpret_cast<char*>(&info), sizeof(tIndexInfo));
        UINT iByteWidth = info.IdxCount * sizeof(UINT);

        void* pSysMem = malloc(iByteWidth);
        info.IdxSysMem = pSysMem;
        file.read(reinterpret_cast<char*>(info.IdxSysMem), iByteWidth);

        D3D11_SUBRESOURCE_DATA tSubData = {};
        tSubData.pSysMem = info.IdxSysMem;

        if (FAILED(DEVICE->CreateBuffer(&info.tIBDesc, &tSubData, info.IB.GetAddressOf())))
        {
            assert(nullptr);
        }
        
		m_vecIdxInfo.push_back(info);
    }

    // Animation3D 정보 
    UINT iCount = 0;
    file.read(reinterpret_cast<char*>(&iCount), sizeof(UINT));
    m_vecAnimClip.resize(iCount);
    for (UINT i = 0; i < iCount; ++i)
    {
        LoadWString(m_vecAnimClip[i].strAnimName, file);
        file.read(reinterpret_cast<char*>(&m_vecAnimClip[i].dStartTime), sizeof(double));
        file.read(reinterpret_cast<char*>(&m_vecAnimClip[i].dEndTime), sizeof(double));
        file.read(reinterpret_cast<char*>(&m_vecAnimClip[i].dTimeLength), sizeof(double));
        file.read(reinterpret_cast<char*>(&m_vecAnimClip[i].eMode), sizeof(int));
        file.read(reinterpret_cast<char*>(&m_vecAnimClip[i].fUpdateTime), sizeof(float));
        file.read(reinterpret_cast<char*>(&m_vecAnimClip[i].iStartFrame), sizeof(int));
        file.read(reinterpret_cast<char*>(&m_vecAnimClip[i].iEndFrame), sizeof(int));
        file.read(reinterpret_cast<char*>(&m_vecAnimClip[i].iFrameLength), sizeof(int));
    }

    iCount = 0;
    file.read(reinterpret_cast<char*>(&iCount), sizeof(UINT));
    m_vecBones.resize(iCount);
    UINT maxFrameCount = 0;
    for (UINT i = 0; i < iCount; ++i)
    {
        LoadWString(m_vecBones[i].strBoneName, file);
        file.read(reinterpret_cast<char*>(&m_vecBones[i].iDepth), sizeof(int));
        file.read(reinterpret_cast<char*>(&m_vecBones[i].iIdx), sizeof(int));
        file.read(reinterpret_cast<char*>(&m_vecBones[i].iParentIndx), sizeof(int));
        file.read(reinterpret_cast<char*>(&m_vecBones[i].matBone), sizeof(Matrix));
        file.read(reinterpret_cast<char*>(&m_vecBones[i].matOffset), sizeof(Matrix));

        int iFrameCount = (int)m_vecBones[i].vecKeyFrame.size();
        file.read(reinterpret_cast<char*>(&iFrameCount), sizeof(int));
        m_vecBones[i].vecKeyFrame.resize(iFrameCount);
        maxFrameCount = max(maxFrameCount, iFrameCount);
        for (int j = 0; j < m_vecBones[i].vecKeyFrame.size(); ++j)
        {
            file.read(reinterpret_cast<char*>(&m_vecBones[i].vecKeyFrame[j]), sizeof(tMTKeyFrame));
        }

        int iChildCount = (int)m_vecBones[i].vecChildIdx.size();
        file.read(reinterpret_cast<char*>(&iChildCount), sizeof(int));
        m_vecBones[i].vecChildIdx.resize(iChildCount);
        for (int j = 0; j < m_vecBones[i].vecChildIdx.size(); ++j)
        {
            file.read(reinterpret_cast<char*>(&m_vecBones[i].vecChildIdx[j]), sizeof(int));
        }
    }

    // Animation 이 있는 Mesh 경우 Bone StructuredBuffer 만들기
    if (m_vecAnimClip.size() > 0 && m_vecBones.size() > 0)
    {
        wstring strBone = GetName();

        // BoneOffet 행렬
        vector<Matrix> vecOffset;
        vector<tFrameTrans> vecFrameTrans;
        vecFrameTrans.resize((UINT)m_vecBones.size() * maxFrameCount);

        for (size_t i = 0; i < m_vecBones.size(); ++i)
        {
            vecOffset.push_back(m_vecBones[i].matOffset);

            for (size_t j = 0; j < m_vecBones[i].vecKeyFrame.size(); ++j)
            {
                vecFrameTrans[(UINT)m_vecBones.size() * j + i]
                    = tFrameTrans{ Vec4(m_vecBones[i].vecKeyFrame[j].vTranslate, 0.f)
                    , Vec4(m_vecBones[i].vecKeyFrame[j].vScale, 0.f)
                    , Vec4(m_vecBones[i].vecKeyFrame[j].qRot) };
            }
        }

        m_BoneInverse = new CStructuredBuffer;
        m_BoneInverse->Create(sizeof(Matrix), (UINT)vecOffset.size(), SB_TYPE::SRV_ONLY, true, vecOffset.data());

        m_BoneFrameData = new CStructuredBuffer;
        m_BoneFrameData->Create(sizeof(tFrameTrans), (UINT)vecOffset.size() * (UINT)maxFrameCount
            , SB_TYPE::SRV_ONLY, false, vecFrameTrans.data());
    }

	file.close();

	DEBUG_NAME(m_VB, "CMesh_VB");

	return S_OK;
}