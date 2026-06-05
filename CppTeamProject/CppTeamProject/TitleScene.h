#pragma once
#include "Scene.h"
#include "Vector2.h"
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
    Vector2 m_resolution;
};

