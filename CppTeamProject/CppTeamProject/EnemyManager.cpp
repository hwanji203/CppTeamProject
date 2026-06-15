#include "EnemyManager.h"
#include "Defines.h"

EnemyManager* EnemyManager::m_inst = nullptr;

void EnemyManager::Init(float spawnDelay)
{
	m_spawnDelay = (ULONGLONG)(spawnDelay * 1000.f); // 초 단위 → ms
	m_nextSpawnTime = GetTickCount64() + m_spawnDelay;
}

void EnemyManager::Update()
{
    for (auto it = m_enemys.begin(); it != m_enemys.end();)
    {
        if (*it != nullptr)
        {
            (*it)->Tick();

            if ((*it)->IsLeaveDeadZone() || (*it)->IsDead())
            {
                (*it)->RemovePrevPos();
                it = m_enemys.erase(it);
                continue;
            }
        }
        ++it;
    }
}

void EnemyManager::Render()
{
    for (auto& enemy : m_enemys)
    {
        if (enemy != nullptr)
        {
            enemy->Render();
        }
    }
}

void EnemyManager::Clear()
{
	// unique_ptr 소멸 → Enemy/Pawn 소멸자가 ColliderManager에서 콜라이더 해제
	m_enemys.clear();
	m_spawnDelay = 0;
	m_nextSpawnTime = 0;
}

void EnemyManager::TrySpawnEnemyInRandomPos(const Vector2* playerPos)
{
    ULONGLONG cur = GetTickCount64();
    if (m_nextSpawnTime < cur)
    {
        m_nextSpawnTime = cur + m_spawnDelay;
        Vector2 startPos = { (std::rand() % (SCREEN_WIDTH / 2)) * 2, 0 };
        auto enemy = std::make_unique<Enemy>(playerPos, 1, startPos);
        m_enemys.push_back(move(enemy));
    }
}