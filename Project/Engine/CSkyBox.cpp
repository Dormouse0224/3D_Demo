#include "pch.h"
#include "CSkyBox.h"
#include "CTransform.h"

#include "CAssetMgr.h"

CSkyBox::CSkyBox()
    : CRenderComponent(COMPONENT_TYPE::SKYBOX)
    , m_SkyboxTex(nullptr)
{
}

CSkyBox::CSkyBox(const CSkyBox& _Other)
    : CRenderComponent(_Other)
    , m_SkyboxTex(_Other.m_SkyboxTex)
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

    // 사용할 재질 바인딩
    // 세팅된 텍스쳐가 큐브 텍스쳐인 경우 텍스쳐 바인딩
    if (m_SkyboxTex.Get() && m_SkyboxTex->GetDesc().MiscFlags & D3D11_RESOURCE_MISC_TEXTURECUBE)
        GetMaterial()->SetTexParam(TEX_PARAM::TEXCUBE_0, m_SkyboxTex);
    GetMaterial()->Binding();

    // 렌더링 시작
    GetMesh()->Render();
}

int CSkyBox::Load(fstream& _Stream)
{
    if (FAILED(CRenderComponent::RenderCom_Load(_Stream)))
        return E_FAIL;

    std::wstring TexName = {};
    int size = 0;
    _Stream.read(reinterpret_cast<char*>(&size), sizeof(int));
    if (size > 0)
    {
        TexName.resize(size);
        _Stream.read(reinterpret_cast<char*>(TexName.data()), sizeof(wchar_t) * size);
        m_SkyboxTex = CAssetMgr::GetInst()->Load<CTexture>(TexName);
    }

    return S_OK;
}

int CSkyBox::Save(fstream& _Stream)
{
    if (FAILED(CRenderComponent::RenderCom_Save(_Stream)))
        return E_FAIL;

    std::wstring TexName = {};
    if (m_SkyboxTex.Get())
        TexName = m_SkyboxTex->GetName();
    int size = TexName.size();
    _Stream.write(reinterpret_cast<char*>(&size), sizeof(int));
    if (size > 0)
        _Stream.write(reinterpret_cast<char*>(TexName.data()), sizeof(wchar_t) * size);

    return S_OK;
}
