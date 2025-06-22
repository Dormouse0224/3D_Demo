#include "pch.h"
#include "CTileRender.h"

#include "CAssetMgr.h"
#include "CTransform.h"

#include "CStructuredBuffer.h"

CTileRender::CTileRender()
	: CRenderComponent(COMPONENT_TYPE::TILERENDER)
	, m_Col(0)
	, m_Row(0)
	, m_TileSize()
	, m_AtlasTex()
	, m_vecTileInfo{}
	, m_GpuBuffer(nullptr)
{

	m_GpuBuffer = new CStructuredBuffer;
}

CTileRender::CTileRender(const CTileRender& _Other)
	: CRenderComponent(_Other)
	, m_Col(_Other.m_Col)
	, m_Row(_Other.m_Row)
	, m_TileSize(_Other.m_TileSize)
	, m_AtlasTex(_Other.m_AtlasTex)
	, m_vecTileInfo{ _Other.m_vecTileInfo }
	, m_GpuBuffer(nullptr)
{

	m_GpuBuffer = new CStructuredBuffer;
}

CTileRender::~CTileRender()
{
	if (nullptr != m_GpuBuffer)
		delete m_GpuBuffer;
}


void CTileRender::FinalTick()
{

}

void CTileRender::Render()
{
	// 위치정보 업데이트
	Transform()->Binding();

	GetMaterial()->SetScalarParam(INT_0, m_Col);
	GetMaterial()->SetScalarParam(INT_1, m_Row);

	GetMaterial()->SetTexParam(TEX_0, m_AtlasTex);

	GetMaterial()->SetScalarParam(VEC2_0, m_TileSize);
	if (m_AtlasTex.Get())
		GetMaterial()->SetScalarParam(VEC2_1, Vec2(m_AtlasTex->GetWidth(), m_AtlasTex->GetHeight()));
	else
		GetMaterial()->SetScalarParam(VEC2_1, Vec2(0, 0));

	GetMaterial()->Binding();

	// 타일정보를 저장하고있는 구조화버퍼 바인딩 - 업데이트 오버헤드가 문제될 수도 있음
	UpdateBuffer();
	m_GpuBuffer->Binding(20);

	// 렌더링 시작
	GetMesh()->Render();
}

int CTileRender::Load(fstream& _Stream)
{
	if (FAILED(CRenderComponent::RenderCom_Load(_Stream)))
		return E_FAIL;


	_Stream.read(reinterpret_cast<char*>(&m_Col), sizeof(UINT));
	_Stream.read(reinterpret_cast<char*>(&m_Row), sizeof(UINT));
	_Stream.read(reinterpret_cast<char*>(&m_TileSize), sizeof(Vec2));

	std::wstring AtlasName = {};
	int size = 0;
	_Stream.read(reinterpret_cast<char*>(&size), sizeof(int));
	if (size > 0)
	{
		AtlasName.resize(size);
		_Stream.read(reinterpret_cast<char*>(AtlasName.data()), sizeof(wchar_t) * size);
		m_AtlasTex = CAssetMgr::GetInst()->Load<CTexture2D>(AtlasName);
	}

	int count = 0;
	_Stream.read(reinterpret_cast<char*>(&count), sizeof(int));
	m_vecTileInfo.resize(count);
	for (int i = 0; i < count; ++i)
	{
		_Stream.read(reinterpret_cast<char*>(&m_vecTileInfo[i]), sizeof(Vec2));
	}

	return S_OK;
}

int CTileRender::Save(fstream& _Stream)
{
	if (FAILED(CRenderComponent::RenderCom_Save(_Stream)))
		return E_FAIL;


	_Stream.write(reinterpret_cast<char*>(&m_Col), sizeof(UINT));
	_Stream.write(reinterpret_cast<char*>(&m_Row), sizeof(UINT));
	_Stream.write(reinterpret_cast<char*>(&m_TileSize), sizeof(Vec2));

	std::wstring AtlasName = {};
	if (m_AtlasTex.Get())
		AtlasName = m_AtlasTex->GetName();
	int size = AtlasName.size();
	_Stream.write(reinterpret_cast<char*>(&size), sizeof(int));
	if (size > 0)
		_Stream.write(reinterpret_cast<char*>(AtlasName.data()), sizeof(wchar_t) * size);

	int count = m_vecTileInfo.size();
	_Stream.write(reinterpret_cast<char*>(&count), sizeof(int));
	for (int i = 0; i < count; ++i)
	{
		_Stream.write(reinterpret_cast<char*>(&m_vecTileInfo[i]), sizeof(Vec2));
	}

	return S_OK;
}


void CTileRender::SetColRow(UINT _Col, UINT _Row)
{
	m_Col = _Col;
	m_Row = _Row;

	// 벡터 크기가 타일 개수보다 큰 경우 벡터 메모리 해제
	if ((m_Col * m_Row) < m_vecTileInfo.capacity())
	{
		vector<TileInfo> temp;
		m_vecTileInfo.swap(temp);
	}

	// 벡터 메모리 크기를 타일 개수만큼 재설정
	m_vecTileInfo.resize(m_Col * m_Row);

	// 벡터 내 모든 LT 값을 (-1, -1) 로 초기화 (해당 인덱스에는 타일이 없다는 뜻)
	for (auto& TileInfo : m_vecTileInfo)
		TileInfo.vLeftTop = Vec2(-1, -1);

	// 오브젝트 스케일 재 조정
	Vec2 vScale = m_TileSize * Vec2(m_Col, m_Row);
	Transform()->SetRelativeScale(Vec3(vScale.x, vScale.y, 1.f));

	// 타일 데이터를 버퍼에 동기화
	UpdateBuffer();
}

void CTileRender::SetTileSize(Vec2 _TileSize)
{
	m_TileSize = _TileSize;

	// 오브젝트 스케일 재 조정
	Vec2 vScale = m_TileSize * Vec2(m_Col, m_Row);
	Transform()->SetRelativeScale(Vec3(vScale.x, vScale.y, 1.f));
}

void CTileRender::SetTile(UINT _Col, UINT _Row, Vec2 _LT)
{
	assert(m_Col > _Col && m_Row > _Row);

	int idx = m_Col * _Row + _Col;
	m_vecTileInfo[idx].vLeftTop = _LT;

	// 타일 데이터를 버퍼에 동기화
	UpdateBuffer();
}

void CTileRender::UpdateBuffer()
{
	if (m_GpuBuffer->GetElementCount() < m_Row * m_Col)
	{
		m_GpuBuffer->Create(sizeof(TileInfo), m_Row* m_Col, SB_TYPE::SRV_ONLY, true);
	}

	
	static vector<TileInfo> vecInfo;
	vecInfo.clear();

	for (size_t i = 0; i < m_vecTileInfo.size(); ++i)
	{
		TileInfo info;
		info.vLeftTop = m_vecTileInfo[i].vLeftTop;
		vecInfo.push_back(info);
	}

	if (m_GpuBuffer->GetElementCount() > 0)
		m_GpuBuffer->SetData(vecInfo.data(), sizeof(TileInfo), vecInfo.size());
}