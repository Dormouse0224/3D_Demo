#pragma once
#include "CAsset.h"

#include "CGraphicShader.h"
#include "CTexture.h"

class CMaterial :
    public CAsset
{
public:
    CMaterial();
    CMaterial(const CMaterial& _Other);
    ~CMaterial();
    CLONE(CMaterial);

private:
    AssetPtr<CGraphicShader>    m_Shader;
    AssetPtr<CTexture>          m_arrTex[TEX_PARAM::TEX_END];
    tMtrlConst                  m_Const;


public:
    AssetPtr<CGraphicShader> GetShader() { return m_Shader; }
    AssetPtr<CTexture> GetTexParam(TEX_PARAM _Idx) { return m_arrTex[_Idx]; }
    tMtrlConst GetConstParam() { return m_Const; }

    void SetShader(AssetPtr<CGraphicShader> _Shader) { m_Shader = _Shader; }
    template<typename T>
    void SetConstParam(CONST_PARAM _Param, const T& _Data);
    void SetTexParam(TEX_PARAM _Param, const AssetPtr<CTexture>& _Tex) { m_arrTex[_Param] = _Tex; nullptr != _Tex ? m_Const.bTex[_Param] = true : m_Const.bTex[_Param] = false; }
    void SetMaterialCoefficient(Vec4 _vDiff, Vec4 _vSpec, Vec4 _vAmb, Vec4 _vEmis);
    void SetAnim3D(bool _bTrue) { m_Const.arrAnimData[0] = (int)_bTrue; }
    void SetBoneCount(int _iBoneCount) { m_Const.arrAnimData[1] = _iBoneCount; }

    void Binding();
    void Unbind();
    static AssetPtr<CMaterial> Create(wstring _Name);

    virtual int Save(const wstring& _FileName, bool _Update = false) override;
    virtual int Load(const wstring& _FilePath) override;
};

template<typename T>
void CMaterial::SetConstParam(CONST_PARAM _Param, const T& _Data)
{
    bool bInput = false;

    switch (_Param)
    {
    case INT_0:
    case INT_1:
    case INT_2:
    case INT_3:
    case INT_4:
    case INT_5:
    {
        if constexpr (std::is_same_v<T, int> || std::is_same_v<T, unsigned int>)
        {
            m_Const.iArr[_Param - INT_0] = _Data;
            bInput = true;
        }
    }
    break;
    case FLOAT_0:
    case FLOAT_1:
    case FLOAT_2:
    case FLOAT_3:
    case FLOAT_4:
    case FLOAT_5:
    {
        if constexpr (std::is_same_v<T, float>)
        {
            m_Const.fArr[_Param - FLOAT_0] = _Data;
            bInput = true;
        }
    }
    break;
    case VEC2_0:
    case VEC2_1:
    case VEC2_2:
    case VEC2_3:
    case VEC2_4:
    case VEC2_5:
    {
        if constexpr (std::is_same_v<T, Vec2>)
        {
            m_Const.v2Arr[_Param - VEC2_0] = _Data;
            bInput = true;
        }
    }
    break;
    case VEC4_0:
    case VEC4_1:
    case VEC4_2:
    case VEC4_3:
    case VEC4_4:
    case VEC4_5:
    {
        if constexpr (std::is_same_v<T, Vec4> || std::is_same_v<T, Vec3>)
        {
            m_Const.v4Arr[_Param - VEC4_0] = _Data;
            bInput = true;
        }
    }
    break;
    case MAT_0:
    case MAT_1:
    case MAT_2:
    case MAT_3:
    case MAT_4:
    case MAT_5:
    {
        if constexpr (std::is_same_v<T, Matrix>)
        {
            m_Const.matArr[_Param - MAT_0] = _Data;
            bInput = true;
        }
    }
    break;
    }

    assert(bInput);
}
