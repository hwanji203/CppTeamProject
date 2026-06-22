#include "EnemyManager.h"
#include "Defines.h"
#include <cstdlib>

EnemyManager* EnemyManager::m_inst = nullptr;

void EnemyManager::Init()
{
	// 스폰 간격은 매개변수가 아니라 Defines의 ENEMY_SPAWN_DELAY(초)에서 가져온다.
	m_spawnDelay = (ULONGLONG)(ENEMY_SPAWN_DELAY * 1000.f); // 초 단위 → ms
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

void EnemyManager::TrySpawnEnemyInRandomPos(const Vector2* playerPos, int groundLength)
{
    ULONGLONG cur = GetTickCount64();
    if (m_nextSpawnTime < cur)
    {
        m_nextSpawnTime = cur + m_spawnDelay;

        // 살아있는 적이 ENEMY_MAX_ALIVE(3)마리 이하일 때만 새로 스폰한다.
        if ((int)m_enemys.size() > ENEMY_MAX_ALIVE)
            return;

        // 지면은 화면 중앙을 기준으로 좌우로 groundLength(타일)만큼 = 화면 2*groundLength 폭.
        // 기존엔 SCREEN_WIDTH/2 ~ +groundLength라 오른쪽에서만 스폰되던 버그 → 좌우 전체로 수정.
        int span   = groundLength * 2;
        int spawnX = SCREEN_WIDTH / 2 - groundLength + (std::rand() % span);

        // 적은 색 구간의 정중앙 값(0.5 / 1.5 / 2.5 / 3.5)으로만 스폰된다.
        int   band  = std::rand() % ENEMY_SPEED_BANDS;
        float speed = band + 0.5f;

        auto enemy = std::make_unique<Enemy>(playerPos, speed, Vector2{ spawnX, 0 });
        m_enemys.push_back(move(enemy));
    }
}