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
    // Scene��(��) ���� ��ӵ�
    void Init() override;
    void Update() override;
    void Render() override;
    void Release() override;
private:
    Vector2 GetSpawnPos() const;
private:
    Vector2 m_resolution;
    std::unique_ptr<Actor> m_player;
    std::unique_ptr<MapGenerator> m_generator;
    std::unique_ptr<GameMap> m_gameMap;
    std::unique_ptr<EnemyManager> m_enemyManager;
};

