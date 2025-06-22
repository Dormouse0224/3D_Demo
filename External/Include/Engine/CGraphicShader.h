#pragma once
#include "CShader.h"

enum SHADER_TYPE
{
    VERTEX_SHADER,
    GEOMETRY_SHADER,
    DOMAIN_SHADER,
    HULL_SHADER,
    PIXEL_SHADER,

    SHADER_TYPE_END
};

extern string SHADER_TYPE_STR[];

class CGraphicShader :
    public CShader
{
public:
    CGraphicShader();
    ~CGraphicShader();
    CLONE_DISABLE(CGraphicShader);

private:
    ComPtr<ID3DBlob>                m_VSBlob;
    ComPtr<ID3DBlob>                m_GSBlob;
    ComPtr<ID3DBlob>                m_DSBlob;
    ComPtr<ID3DBlob>                m_HSBlob;
    ComPtr<ID3DBlob>                m_PSBlob;

    ComPtr<ID3D11VertexShader>      m_VS;
    ComPtr<ID3D11GeometryShader>    m_GS;
    ComPtr<ID3D11DomainShader>      m_DS;
    ComPtr<ID3D11HullShader>        m_HS;
    ComPtr<ID3D11PixelShader>       m_PS;
    pair<wstring, string>           m_ShaderName[SHADER_TYPE_END];  // 셰이더 파일 디렉토리와 셰이더 함수 이름

    // 정점 구조체 구조정보 레이아웃
    ComPtr<ID3D11InputLayout>       m_Layout;
    D3D11_PRIMITIVE_TOPOLOGY        m_Topology;

    SHADER_DOMAIN                   m_Domain;

    RS_TYPE                         m_RSType;
    DS_TYPE                         m_DSType;
    BS_TYPE                         m_BSType;


public:
    pair<wstring, string> GetShaderName(SHADER_TYPE _Type) { return m_ShaderName[_Type]; }
    D3D11_PRIMITIVE_TOPOLOGY GetTopology() { return m_Topology; }
    SHADER_DOMAIN GetDomain() { return m_Domain; }
    RS_TYPE GetRSType() { return m_RSType; }
    DS_TYPE GetDSType() { return m_DSType; }
    BS_TYPE GetBSType() { return m_BSType; }

    void SetTopology(D3D11_PRIMITIVE_TOPOLOGY _Topology) { m_Topology = _Topology; }
    void SetRSType(RS_TYPE _Type) { m_RSType = _Type; }
    void SetDSType(DS_TYPE _Type) { m_DSType = _Type; }
    void SetBSType(BS_TYPE _Type) { m_BSType = _Type; }
    void SetDomain(SHADER_DOMAIN _Domain) { m_Domain = _Domain; }

    int CreateVertexShader(const wstring& _RelativePath, const string& _FuncName);
    int CreateGeometryShader(const wstring& _RelativePath, const string& _FuncName);
    int CreateDomainShader(const wstring& _RelativePath, const string& _FuncName);
    int CreateHullShader(const wstring& _RelativePath, const string& _FuncName);
    int CreatePixelShader(const wstring& _RelativePath, const string& _FuncName);

    void Binding();

    virtual int Save(const wstring& _FileName, bool _Update = false) override;
    virtual int Load(const wstring& _FilePath) override;

};

