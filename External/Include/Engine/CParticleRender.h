#pragma once
#include "CRenderComponent.h"
#include "CParticleRenderUI.h"
#include "CParticleTickCS.h"

class CStructuredBuffer;

class CParticleRender :
    public CRenderComponent
{
    friend class CParticleRenderUI;
public:
    CParticleRender();
    CParticleRender(const CParticleRender& _Other);
    ~CParticleRender();
    CLONE(CParticleRender);

private:
    CStructuredBuffer*          m_ParticleBuffer;
    CStructuredBuffer*          m_SpawnBuffer;
    CStructuredBuffer*          m_ModuleBuffer;

    AssetPtr<CParticleTickCS>   m_TickCS;
    UINT                        m_MaxParticle;      // 파티클 최대 개수
    tParticleModule             m_Module;           // 파티클 제어 설정값
    float                       m_AccTime;          // 누적시간 체크
    int                         m_SpawnCount;
    bool                        m_Active;
    bool                        m_Gravity;

    AssetPtr<CTexture2D>        m_ParticleTex;

    map<UINT, CGameObject*>     m_mapParticleObj;
    int                         m_PrevSpaceType;

public:
    void SetSpawnRate(float _Rate) { m_Module.SpawnRate = _Rate; }
    void SetSpawnCount(float _Count) { m_SpawnCount = _Count; }
    void SetParticleTex(const AssetPtr<CTexture2D>& _Tex) { m_ParticleTex = _Tex; }
    float GetSpawnRate() { return m_Module.SpawnRate; }
    void SetStartColor(Vec4 _Color) { m_Module.StartColor = _Color; }
    void SetEndColor(Vec4 _Color) { m_Module.EndColor = _Color; }
    void SetMinScale(Vec3 _Scale) { m_Module.MinScale = _Scale; }
    void SetMaxScale(Vec3 _Scale) { m_Module.MaxScale = _Scale; }
    void SetMinSpeed(float _Speed) { m_Module.MinSpeed = _Speed; }
    void SetMaxSpeed(float _Speed) { m_Module.MaxSpeed = _Speed; }
    void SetMinLife(float _MinLife) { m_Module.MinLife = _MinLife; }
    void SetMaxLife(float _MaxLife) { m_Module.MaxLife = _MaxLife; }
    // 0 : Box, 1 : Sphere
    void SetSpawnShape(int _Shape) { m_Module.SpawnShape = _Shape; }
    void SetSpawnShapeScale(Vec3 _Scale) { m_Module.SpawnShapeScale = _Scale; }
    void SetSpawnDir(Vec3 _Dir) { m_Module.SpawnDir = _Dir; }
    void SetSpawnDirRandomize(float _Randomize) { m_Module.SpawnDirRandomize = _Randomize; }
    // 0 : Local, 1 : World;
    void SetSpaceType(int _Type) { m_Module.SpaceType = _Type; }
    void SetActiveState(bool _b) { m_Active = _b; }
    void SetGravityState(bool _b) { m_Gravity = _b; }





public:
    virtual void FinalTick() override;
    virtual void Render() override;

    virtual int Load(fstream& _Stream) override;
    virtual int Save(fstream& _Stream) override;

private:
    void CreateMtrl();
    void CalcSpawnCount();


};

