#pragma once
#include "CComponent.h"

class CTransform :
    public CComponent
{
public:
    CTransform();
    ~CTransform();
    CLONE(CTransform);

private:
    Vec3    m_RelativePos;  // 위치
    Vec3    m_RelativeScale;// 크기
    Vec4    m_RelativeRot;  // 회전(Quaternion)

    Vec3    m_RelativeDir[(UINT)DIR::END]; // 오브젝트의 방향정보
    Vec3    m_WorldDir[(UINT)DIR::END]; // 오브젝트의 방향정보

    Matrix  m_matWorld;     // 월드 변환행렬

    // 자식 오브젝트일때 부모의 크기에 영향을 받지않는 독립된 크기를 가진다.
    bool    m_IndependentScale;
    bool    m_IndependentRot;
    bool    m_IndependentTrans;


public:
    virtual void FinalTick() override;
    void Binding();

public:
    void SetRelativePos(Vec3 _Pos);
    void SetRelativePos(float _x, float _y, float _z) { SetRelativePos(Vec3(_x, _y, _z)); }

    void SetRelativeScale(Vec3 _Scale) { m_RelativeScale = _Scale; }
    void SetRelativeScale(float _x, float _y, float _z) { m_RelativeScale = Vec3(_x, _y, _z); }

    void SetRelativeRot(Vec4 _RotationQuat);
    void SetRelativeRot(Vec3 _RotationDeg);
    void SetRelativeRot(float _xDeg, float _yDeg, float _zDeg);
    //void SetRelativeRotation(PxQuat _Rotation);

    void SetIndependentScale(bool _Scale) { m_IndependentScale = _Scale; }
    void SetIndependentRot(bool _Rot) { m_IndependentRot = _Rot; }
    void SetIndependentTrans(bool _Trans) { m_IndependentTrans = _Trans; }

    Vec3 GetRelativePos() { return m_RelativePos; }
    Vec3 GetRelativeScale() { return m_RelativeScale; }

    Vec3 GetWorldTrans();
    Vec3 GetWorldScale();
    Vec4 GetWorldRot();

    Vec3 GetRelativeRotEuler();
    Vec4 GetRelativeRotQuat() { return m_RelativeRot; }
    //Vec3 GetWorldRotation();

    Vec3 GetRelativeDir(DIR _DirType) { return m_RelativeDir[(UINT)_DirType]; }
    Vec3 GetWorldDir(DIR _DirType) { return m_WorldDir[(UINT)_DirType]; }

    bool GetIndependentScale() { return m_IndependentScale; }
    bool GetIndependentRot() { return m_IndependentRot; }
    bool GetIndependentTrans() { return m_IndependentTrans; }

    const Matrix& GetWorldMat() { return m_matWorld; }
    void SetWorldMat(const Matrix& _Mat) { m_matWorld = _Mat; }

    virtual int Load(fstream& _Stream) override;
    virtual int Save(fstream& _Stream) override;

private:
    void CalcMatWorld();
};

