#pragma once
#include "Scene.h"
#include "Position.h"
class TitleScene : public Scene
{
public:
    // Scene을(를) 통해 상속됨
    void Init() override;
    void Update() override;
    void Render() override;
    void Release() override;
private:
    void EnterAnimation();
    Position m_resolution;
};

