#include "pch.h"
#include "CMeshRender.h"

#include "CMesh.h"
#include "CMaterial.h"
#include "CTransform.h"
#include "CAnimator3D.h"

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

    for (UINT i = 0; i < GetMaterialCount(); ++i)
    {
        AssetPtr<CMaterial> Mtrl = GetMaterial(i);
        if (nullptr == Mtrl)
            continue;

        if (Animator3D())
            Animator3D()->Binding(Mtrl);

        // 사용할 재질 바인딩
        Mtrl->Binding();

        // 렌더링 시작
        GetMesh()->Render(i);
    }

    if (Animator3D())
        Animator3D()->ClearData();
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
