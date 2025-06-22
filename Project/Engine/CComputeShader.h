#pragma once
#include "CShader.h"
#include "CStructuredBuffer.h"

#include "CTexture2D.h"

class CComputeShader :
    public CShader
{
public:
    CComputeShader();
    CComputeShader(const wstring& _RelativePath, const string& _FuncName
        , UINT _GroupPerX, UINT _GroupPerY, UINT _GroupPerZ);
    ~CComputeShader();
    CLONE_DISABLE(CComputeShader);

protected:
    ComPtr<ID3DBlob>                m_CSBlob;
    ComPtr<ID3D11ComputeShader>     m_CS;

    tMtrlConst                      m_Const;
    pair<AssetPtr<CTexture2D>, int> m_CSTex[TEX_PARAM::TEX_END];    // 텍스쳐 셰이더 리소스와 텍스쳐가 바인딩 될 레지스터 넘버

    const UINT                      m_ThreadPerGroupX;
    const UINT                      m_ThreadPerGroupY;
    const UINT                      m_ThreadPerGroupZ;

    UINT                            m_GroupX;
    UINT                            m_GroupY;
    UINT                            m_GroupZ;


protected:
    int CreateComputeShader(const wstring& _RelativePath, const string& _FuncName);

public:
    tMtrlConst GetConstParam() { return m_Const; }
    pair<AssetPtr<CTexture2D>, int>& GetCSTex(TEX_PARAM _Param) { return m_CSTex[_Param]; }
    Vec3 GetThreadPerGroup() { return Vec3(m_ThreadPerGroupX, m_ThreadPerGroupY, m_ThreadPerGroupZ); }
    Vec3 GetGroupCount() { return Vec3(m_GroupX, m_GroupY, m_GroupZ); }

    int Execute();
    virtual void UIRender() = 0;
    virtual void SetData() = 0;

private:
    virtual int Binding() = 0;
    virtual void CalcGroupNum() = 0;
    virtual void Clear() = 0;

public:
    virtual int Save(const wstring& _FilePath, bool _Update = false) { MessageBoxW(nullptr, L"컴퓨트 셰이더는 파일 저장을 지원하지 않습니다.", L"Compute Shader Save Error", MB_OK); return S_OK; }
    virtual int Load(const wstring& _FilePath) { return S_OK; }
};

