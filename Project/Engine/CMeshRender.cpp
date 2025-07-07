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

    // Animator3D Binding
    if (Animator3D())
    {
        Animator3D()->Binding();

        for (UINT i = 0; i < GetMesh()->GetIdxCount(); ++i)
        {
            if (nullptr == GetMaterial(i))
                continue;

            GetMaterial(i)->SetAnim3D(true); // Animation Mesh �˸���
            GetMaterial(i)->SetBoneCount(Animator3D()->GetBoneCount());
        }
    }

    for (UINT i = 0; i < GetMaterialCount(); ++i)
    {
        if (nullptr == GetMaterial(i))
            continue;

        // ����� ���� ���ε�
        GetMaterial(i)->Binding();

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
