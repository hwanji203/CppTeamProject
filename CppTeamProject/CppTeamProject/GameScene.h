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

    // 설정창(일시정지) 동안 생존 시간 타이머를 멈췄다 재개한다.
    void Pause();
    void Resume();
private:
    Vector2 GetSpawnPos() const;
    void DrawHud() const;
private:
    unsigned long long m_startTick = 0;
    unsigned long long m_pauseStart = 0;
    Vector2 m_resolution;
    std::unique_ptr<Actor> m_player;
    std::unique_ptr<MapGenerator> m_generator;
    std::unique_ptr<GameMap> m_gameMap;
    std::unique_ptr<EnemyManager> m_enemyManager;
};

