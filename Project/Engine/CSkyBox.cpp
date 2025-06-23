#include "pch.h"
#include "CSkyBox.h"
#include "CTransform.h"

#include "CAssetMgr.h"

CSkyBox::CSkyBox()
    : CRenderComponent(COMPONENT_TYPE::SKYBOX)
{
}

CSkyBox::~CSkyBox()
{
}

void CSkyBox::FinalTick()
{
    if (GetMesh() == nullptr || GetMesh()->GetName() != L"EA_CubeMesh")
        SetMesh(CAssetMgr::GetInst()->Load<CMesh>(L"EA_CubeMesh", true));
    if (GetMaterial() == nullptr || GetMaterial()->GetName() != L"EA_SkyboxMtrl")
        SetMaterial(CAssetMgr::GetInst()->Load<CMaterial>(L"EA_SkyboxMtrl", true));
}

void CSkyBox::Render()
{
    // ��ġ���� ������Ʈ
    Transform()->Binding();

    // ����� ���̴� ���ε�
    GetMaterial()->Binding();

    // ������ ����
    GetMesh()->Render();
}

int CSkyBox::Load(fstream& _Stream)
{
    if (FAILED(CRenderComponent::RenderCom_Load(_Stream)))
        return E_FAIL;

    return S_OK;
}

int CSkyBox::Save(fstream& _Stream)
{
    if (FAILED(CRenderComponent::RenderCom_Save(_Stream)))
        return E_FAIL;

    return S_OK;
}
