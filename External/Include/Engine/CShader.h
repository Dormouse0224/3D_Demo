#pragma once
#include "CAsset.h"


struct tConstData
{
    CONST_PARAM m_Param;
    string      m_Desc;
};

struct tTexData
{
    TEX_PARAM   m_Param;
    string      m_Desc;
};

class CShader :
    public CAsset
{
public:
    CShader(ASSET_TYPE _Type);
    ~CShader();
    CLONE_DISABLE(CShader);

protected:
    ComPtr<ID3DBlob>                m_ErrBlob;

    vector<tConstData*>              m_vecConstData;
    vector<tTexData*>                m_vecTexData;

public:
    vector<tConstData*> GetConstData() { return m_vecConstData; }
    vector<tTexData*> GetTexData() { return m_vecTexData; }

    void SetConstData(CONST_PARAM _Param, string _Desc)
    {
        tConstData* data = new tConstData;
        data->m_Param = _Param;
        data->m_Desc = _Desc;
        m_vecConstData.push_back(data);
    }
    void SetTexData(TEX_PARAM _Param, string _Desc)
    {
        tTexData* data = new tTexData;
        data->m_Param = _Param;
        data->m_Desc = _Desc;
        m_vecTexData.push_back(data);
    }

};

