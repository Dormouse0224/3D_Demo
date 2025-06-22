#pragma once
#include "CRenderComponent.h"
#include "CFlipbookRenderUI.h"

#include "CFlipbook.h"

class CFlipbookRender :
    public CRenderComponent
{
    friend class CFlipbookRenderUI;
public:
    CFlipbookRender();
    ~CFlipbookRender();
    CLONE(CFlipbookRender);

private:
    vector<AssetPtr<CFlipbook>>     m_vecFlipbook;

    // 재생 정보
    AssetPtr<CFlipbook>             m_CurFlipbook;
    int                             m_SceneIdx;
    float                           m_Time;
    float                           m_FPS;
    bool                            m_Repeat;
    bool                            m_Finish;
    bool                            m_Pause;


public:
    void AddFlipbook(AssetPtr<CFlipbook> _Flipbook);
    void DeleteFlipbook(int _FlipIdx);
    void Play(int _Idx)
    {
        m_Pause = false;
        if (_Idx >= m_vecFlipbook.size())
            return;
        m_CurFlipbook = m_vecFlipbook[_Idx];
    }
    void Play(wstring _Name, float _FPS, bool _Repeat, int _SceneIdx = 0)
    {
        m_Pause = false;
        for (auto flip : m_vecFlipbook)
        {
            if (flip->GetName() == _Name)
            {
                m_CurFlipbook = flip;
                m_FPS = _FPS;
                m_Time = 0.f;
                m_Finish = false;
                m_Repeat = _Repeat;
                m_SceneIdx = _SceneIdx;
                break;
            }
        }
    }
    bool IsFinish() { return m_Finish; }
    void Pause(bool _b) { m_Pause = _b; }
    void Stop() { m_Pause = false; m_CurFlipbook = nullptr; }


public:
    virtual void FinalTick() override;
    virtual void Render() override;

    virtual int Load(fstream& _Stream) override;
    virtual int Save(fstream& _Stream) override;


};

