#pragma once
#include "CEntity.h"

class CCamera;


enum class FACE_TYPE
{
    eLEFT,
    eRIGHT,
    eBOTTOM,
    eTOP,

    eNEAR_0,
    eFAR_0,

    eNEAR_1,
    eFAR_1,

    eNEAR_2,
    eFAR_2,

    eEND,
};

enum class CASCADE
{
    eNEAR,
    eMIDDLE,
    eFAR,

    eEND,
};

class CFrustum :
    public CEntity
{
    friend class CCamera;
public:
    CFrustum();
    virtual ~CFrustum();
    CLONE(CFrustum);

private:
    CCamera*    m_Owner;

    Vec3        m_ProjPos[8];
    Vec4        m_Face[(UINT)FACE_TYPE::eEND];


public:
    bool FrustumCheck(Vec3 _WorldPos, CASCADE _Cascade);
    bool FrustumCheckAll(Vec3 _WorldPos);
    bool FrustumCheckSphere(Vec3 _WorldPos, float _Radius, CASCADE _Cascade);
    bool FrustumCheckSphereAll(Vec3 _WorldPos, float _Radius);


private:
    void FinalTick();

};

