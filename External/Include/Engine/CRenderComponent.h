#pragma once
#include "CComponent.h"
#include "CRenderComponentUI.h"

#include "CMesh.h"
#include "CMaterial.h"

struct tMtrlSet
{
    AssetPtr<CMaterial>     pSharedMtrl;    // ���� ���׸���
    AssetPtr<CMaterial>     pDynamicMtrl;   // ���� ���׸����� ���纻    
    bool                    bUsingDynamic;
};

class CRenderComponent :
    public CComponent
{
    friend class CRenderComponentUI;
public:
    CRenderComponent(COMPONENT_TYPE _Type);
    CRenderComponent(const CRenderComponent& _Other);
    ~CRenderComponent();

private:
    AssetPtr<CMesh>     m_Mesh;
    vector<tMtrlSet>    m_vecMtrls; // �޽ÿ� �����ϴ� ������


public:
    void SetMesh(AssetPtr<CMesh> _Mesh) { m_Mesh = _Mesh; }
    void SetMaterial(AssetPtr<CMaterial> _Mtrl, UINT _Idx = 0);
    void SetUsingDynamic(bool _UsingDynamic, UINT _Idx);

    AssetPtr<CMesh> GetMesh() { return m_Mesh; }
    // ���� RenderComponent �� ���� ���� ��� ���ο� ���� �⺻ ���� �Ǵ� ���� ������ ��ȯ�մϴ�.
    AssetPtr<CMaterial> GetMaterial(UINT _Idx = 0);
    AssetPtr<CMaterial> GetSharedMaterial(UINT _Idx);
    bool GetUsingDynamic(UINT _Idx);
    UINT GetMaterialCount() { return (UINT)m_vecMtrls.size(); }
    

    virtual void Render() = 0;

protected:
    int RenderCom_Load(fstream& _Stream);
    int RenderCom_Save(fstream& _Stream);
};

