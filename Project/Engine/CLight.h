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

    AssetPtr<CMesh>			m_LightMesh;    // 빛이 닿는 영역에 픽셀 셰이더를 호출시킬 메시
    AssetPtr<CMaterial>		m_LightMtrl;

    // 그림자맵 텍스쳐
    CMRT*           m_DirectionalShadowMapMRT;
    CMRT*           m_PointShadowMapMRT;
    CMRT*           m_SpotShadowMapMRT;
    CGameObject*    m_LightCam;

    tShadowMat      m_ShadowMat;

    bool            m_CSM;

public:
    virtual void FinalTick();

    void Render();
    void ShadowRender();
    void SetMatrixShadow();
    Matrix CalcDirectionalVP(Vec4 _CircumscribedSphere, Vec3 _Dir);
    Matrix CalcPointVP(Vec3 _LightDir, Vec3 _UpDir);
    Matrix CalcSpotlightVP();

    tLightModule GetLightModule() { return m_LightModule; }
    int GetLightIdx() { return m_LightIdx; }
    bool GetCSM() { return m_CSM; }

    void SetLightModule(tLightModule _Module);
    void SetLightIdx(int _LightIdx) { m_LightIdx = _LightIdx; }
    void SetCSM(bool _CSM) { m_CSM = _CSM; }

    virtual int Save(fstream& _Stream);
    virtual int Load(fstream& _Stream);
};

