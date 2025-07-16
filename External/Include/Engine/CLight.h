#pragma once
#include "CComponent.h"

class CMesh;
class CMaterial;
class CMRT;

class CLight :
    public CComponent
{
public:
    CLight();
    CLight(const CLight& _Origin);
    ~CLight();
    CLONE(CLight);

private:
    tLightModule m_LightModule;
    int m_LightIdx;

    AssetPtr<CMesh>			m_LightMesh;    // ���� ��� ������ �ȼ� ���̴��� ȣ���ų �޽�
    AssetPtr<CMaterial>		m_LightMtrl;

    // �׸��ڸ� �ؽ���
    CMRT*           m_DirectionalShadowMapMRT;
    CMRT*           m_PointShadowMapMRT;
    CMRT*           m_SpotShadowMapMRT;
    CGameObject*    m_LightCam;

public:
    virtual void FinalTick();

    void Render();
    void ShadowRender();

    tLightModule GetLightModule() { return m_LightModule; }
    int GetLightIdx() { return m_LightIdx; }

    void SetLightModule(tLightModule _Module);
    void SetLightIdx(int _LightIdx) { m_LightIdx = _LightIdx; }

    virtual int Save(fstream& _Stream);
    virtual int Load(fstream& _Stream);
};

