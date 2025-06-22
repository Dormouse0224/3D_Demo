#pragma once
#include "CEntity.h"

class CGameObject;

class CLayer :
    public CEntity
{
    //friend class CLevel;
public:
    CLayer();
    CLayer(const CLayer& _Other);
    ~CLayer();
    CLONE(CLayer);

private:
    vector<CGameObject*>    m_vecParentObjects; // 레이어에 소속된 오브젝트 중, 최상위 부모 오브젝트만 관리
    // 최상위 부모, 자식 상관없이, 해당 레이어 소속 모든 오브젝트.
    // 매 틱마다 클리어되고, 모든 오브젝트의 FinalTick 시점에 오브젝트는 자신의 레이어에 스스로를 RegisterGameObject 로 등록함.
    vector<CGameObject*>    m_vecObjects;
    int                     m_LayerIdx;         // 레이어 번호

public:
    void Begin();
    void End();
    void Tick();
    void Register();
    void FinalTick();

public:
    const vector<CGameObject*>& GetParentObjects() { return m_vecParentObjects; }
    const vector<CGameObject*>& GetObjects() { return m_vecObjects; }

    void SetLayerIdx(int _Idx) { m_LayerIdx = _Idx; }

    void AddGameObject(CGameObject* _Parent, bool _bChildMove);
    void RegisterGameObject(CGameObject* _Object) { m_vecObjects.push_back(_Object); }
    void UnRegisterParentObject(CGameObject* _Parent);
    void ClearVecObjects() { m_vecObjects.clear(); }

    int Save(fstream& _File, bool _Update = false);
    int Load(fstream& _File);

};

