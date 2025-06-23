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
    // 위치정보 업데이트
    Transform()->Binding();

    // 사용할 쉐이더 바인딩
    GetMaterial()->Binding();

    // 렌더링 시작
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
