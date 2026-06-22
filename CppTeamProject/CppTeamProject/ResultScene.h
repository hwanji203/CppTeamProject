#pragma once
#include "Scene.h"

// 플레이어 사망 후 표시되는 결과 화면.
// 버틴 시간을 보여주고 Restart / Leave 중 선택하게 한다.
class ResultScene : public Scene
{
public:
    void Init() override;
    void Update() override;
    void Render() override;
    void Release() override;

    void SetSurviveTime(float seconds) { m_surviveTime = seconds; }

private:
    void DrawBox();
    void DrawContents();

private:
    float m_surviveTime = 0.f;   // 버틴 시간(초)
    int   m_selectedIdx = 0;     // 0 = Restart, 1 = Leave
    bool  m_justOpened  = true;  // 진입 첫 프레임 입력 무시(직전 키 입력 차단)
};
