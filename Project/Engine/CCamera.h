#pragma once
#include "CComponent.h"

class CCamera :
    public CComponent
{
    friend class CRenderMgr;
public:
    CCamera();
    CCamera(const CCamera& _Origin);
    ~CCamera();
    CLONE(CCamera);

private:
    PROJ_TYPE   m_ProjType;     // 투영 방법
    float       m_ViewX;        // 카메라 투영 가로길이
    float       m_ViewY;        // 카메라 투영 세로길이
    float       m_FOV;          // 시야각(FieldOfView)
    float       m_Far;          // 최대 시야거리

    Matrix      m_matView;      // View 행렬
    Matrix      m_matProj;      // 투영행렬

    int         m_Priority;     // 카메라 우선순위, -1 : 미등록 카메라, 0 : 메인 카메라, 1 ~> : 서브 카메라
    bool        m_Registered;
    float       m_Zoom;

    UINT        m_LayerCheck;   // 카메라가 렌더링할 레이어 비트설정


    // 물체 분류 용도
    vector<CGameObject*>                m_vecOpaque;
    vector<CGameObject*>                m_vecMasked;
    vector<pair<float, CGameObject*>>   m_vecTransparent;
    vector<CGameObject*>                m_vecPostprocess;
    vector<CGameObject*>                m_vecUI;



public:
    void SetProjType(PROJ_TYPE _Type) { m_ProjType = _Type; }
    void SetViewX(float _Width) { m_ViewX = _Width; }
    void SetViewY(float _Height) { m_ViewY = _Height; }
    void SetFOV(float _FOV) { m_FOV = (_FOV / 180.f) * XM_PI; }
    void SetFar(float _Far) { m_Far = _Far; }
    void SetPriority(int _Priority);
    void SetZoom(float _Zoom) { m_Zoom = _Zoom; }

    PROJ_TYPE GetProjType() { return m_ProjType; }
    float GetViewX() { return m_ViewX; }
    float GetViewY() { return m_ViewY; }
    float GetFOV() { return (m_FOV / XM_PI) * 180.f; }
    float GetFar() { return m_Far; }
    int GetPriority() { return m_Priority; }
    bool GetLayerState(LAYER _LayerIdx) { return m_LayerCheck & (1 << (UINT)_LayerIdx); }
    float GetZoom() { return m_Zoom; }

    void CheckLayer(LAYER _LayerIdx) { m_LayerCheck ^= (1 << (UINT)_LayerIdx); }
    void CheckLayerAll() { m_LayerCheck = 0xffffffff; }
    void CheckLayerClear() { m_LayerCheck = 0; }

    const Matrix& GetViewMat() { return m_matView; }
    const Matrix& GetProjMat() { return m_matProj; }



private:
    void SortObject();

public:
    virtual void FinalTick() override;
    void SetMatrix();
    void Render();

    /// <summary>
    /// Render Manager 에서 관리되지 않는 별도의 카메라 객체용 렌더링 함수입니다.
    /// </summary>
    /// <param name="_vecObj">렌더링 대상 오브젝트 벡터</param>
    void Direct_Render(const vector<CGameObject*>& _vecObj);

    virtual int Load(fstream& _Stream) override;
    virtual int Save(fstream& _Stream) override;
};