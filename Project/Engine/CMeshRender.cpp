#include "pch.h"
#include "CMeshRender.h"

#include "CMesh.h"
#include "CMaterial.h"
#include "CTransform.h"

CMeshRender::CMeshRender()
	: CRenderComponent(COMPONENT_TYPE::MESHRENDER)
{
}

CMeshRender::~CMeshRender()
{
}


void CMeshRender::FinalTick()
{
}

void CMeshRender::Render()
{
	// 위치정보 업데이트
	Transform()->Binding();

	// 사용할 쉐이더 바인딩
	GetMaterial()->Binding();

	// 렌더링 시작
	GetMesh()->Render();
}

int CMeshRender::Load(fstream& _Stream)
{
	if (FAILED(CRenderComponent::RenderCom_Load(_Stream)))
		return E_FAIL;

	return S_OK;
}

int CMeshRender::Save(fstream& _Stream)
{
	if (FAILED(CRenderComponent::RenderCom_Save(_Stream)))
		return E_FAIL;

	return S_OK;
}
