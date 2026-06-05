#pragma once
#include "Scene.h"
#include<memory>
#include "Vector2.h"
#include "Actor.h"
#include "InputHandler.h"
#include "MapGenerator.h"
#include "GameMap.h"
class GameScene :
    public Scene
{
public:
    // Scene을(를) 통해 상속됨
    void Init() override;
    void Update() override;
    void Render() override;
    void Release() override;
private:
    Vector2 GetSpawnPos() const;
private:
    Vector2 m_resolution;
    std::unique_ptr<Actor> m_player;
    std::unique_ptr<InputHandler> m_inputHandler;
    std::unique_ptr<MapGenerator> m_generator;
    std::unique_ptr<GameMap> m_gameMap;
};

