#pragma once
#include "Scene.h"
#include<memory>
#include "Vector2.h"
#include "Actor.h"
#include "MapGenerator.h"
#include "GameMap.h"
#include "EnemyManager.h"

class GameScene :
    public Scene
{
public:
    void Init() override;
    void Update() override;
    void Render() override;
    void Release() override;
private:
    Vector2 GetSpawnPos() const;
    void DrawHud() const;
private:
    unsigned long long m_startTick = 0;
    Vector2 m_resolution;
    std::unique_ptr<Actor> m_player;
    std::unique_ptr<MapGenerator> m_generator;
    std::unique_ptr<GameMap> m_gameMap;
    std::unique_ptr<EnemyManager> m_enemyManager;
};

