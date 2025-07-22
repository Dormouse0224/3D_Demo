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
    // ��ġ���� ������Ʈ
    Transform()->Binding();

    for (UINT i = 0; i < GetMaterialCount(); ++i)
    {
        AssetPtr<CMaterial> Mtrl = GetMaterial(i);
        if (nullptr == Mtrl)
            continue;

        if (Animator3D())
            Animator3D()->Binding(Mtrl);

        // ����� ���� ���ε�
        Mtrl->Binding();

        // ������ ����
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
