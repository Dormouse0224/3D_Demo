#pragma once

#include "CEntity.h"

class CComponent;
class CRenderComponent;
class CScript;


class CGameObject :
    public CEntity
{
    friend class CLayer;
    friend class CTaskMgr;
public:
    CGameObject();
    CGameObject(const CGameObject& _Other);
    ~CGameObject();
    CLONE(CGameObject);

private:
    CComponent*             m_Com[(UINT)COMPONENT_TYPE::COMPONENT_END];
    vector<CScript*>        m_vecScript;
    map<string, CScript*>   m_mapScript;
    CRenderComponent*       m_RenderCom; // 렌더링 컴포넌트 포인터

    CGameObject*            m_Parent;   // 부모 오브젝트
    vector<CGameObject*>    m_vecChild; // 자식 오브젝트

    int                     m_LayerIdx; // 소속 레이어 인덱스, -1 : 어느 레이어에도 소속되어있지 않다.

    bool                    m_Dead;  // 오브젝트가 삭제되기 직전 상태 체크


public:
    CComponent* GetComponent(COMPONENT_TYPE _Type) { return m_Com[(UINT)_Type]; }
    CRenderComponent* GetRenderComponent() { return m_RenderCom; }
    void AddComponent(CComponent* _Component);

    bool IsDead() { return m_Dead; }

    void AddChild(CGameObject* _Child);
    void DeleteChild(CGameObject* _Child);

    CGameObject* GetParent() const { return m_Parent; }
    const vector<CGameObject*>& GetChild() const { return m_vecChild; } // 자식 오브젝트
    CGameObject* FindChild(wstring _Name);
    const vector<CScript*>& GetScripts() { return m_vecScript; }
    int GetLayerIdx() { return m_LayerIdx; }

    void SetLayerIdx(int _Idx) { m_LayerIdx = _Idx; }

    class CTransform*   Transform() { return (CTransform*)GetComponent(COMPONENT_TYPE::TRANSFORM); }
    class CCamera*      Camera() { return (CCamera*)GetComponent(COMPONENT_TYPE::CAMERA); }
    class CPhysxActor*  PhysxActor() { return (CPhysxActor*)GetComponent(COMPONENT_TYPE::PHYSXACTOR); }
    class CFSM*         FSM() { return (CFSM*)GetComponent(COMPONENT_TYPE::FSM); }
    class CUICom*       UICom() { return (CUICom*)GetComponent(COMPONENT_TYPE::UICOM); }
    class CLight*       Light() { return (CLight*)GetComponent(COMPONENT_TYPE::LIGHT); }

    class CMeshRender*      MeshRender() { return (CMeshRender*)GetComponent(COMPONENT_TYPE::MESHRENDER); }
    class CSpriteRender*    SpriteRender() { return (CSpriteRender*)GetComponent(COMPONENT_TYPE::SPRITERENDER); }
    class CFlipbookRender*  FlipbookRender() { return (CFlipbookRender*)GetComponent(COMPONENT_TYPE::FLIPBOOKRENDER); }
    class CTileRender*      TileRender() { return (CTileRender*)GetComponent(COMPONENT_TYPE::TILERENDER); }
    class CParticleRender*  ParticleRender() { return (CParticleRender*)GetComponent(COMPONENT_TYPE::PARTICLERENDER); }

    void Destroy();
    void ChangeLayer(LAYER _Dest, bool _CHildMove);
    // 호출자가 대상 오브젝트의 조상인지 검사합니다.
    bool IsAncestorOf(CGameObject* _Obj);
    void ConvertToRoot();
    CScript* FindScript(string _className);

public:
    void Begin();
    void End();
    void Tick();

    void RegisterToLevel();
    void FinalTick();
    void Render();

    int Save(fstream& _Stream, bool _Update = false);
    int Load(fstream& _Stream);
};  

