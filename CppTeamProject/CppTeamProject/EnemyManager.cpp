#include "EnemyManager.h"
#include "Defines.h"
#include <cstdlib>

EnemyManager* EnemyManager::m_inst = nullptr;

void EnemyManager::Init()
{
	// 스폰 간격은 시간이 지날수록 ENEMY_SPAWN_DELAY -> ENEMY_SPAWN_DELAY_MIN으로 줄어든다.
	m_startTime = GetTickCount64();
	m_spawnDelay = (ULONGLONG)(ENEMY_SPAWN_DELAY * 1000.f); // 초 단위 -> ms
	m_nextSpawnTime = m_startTime + m_spawnDelay;
}

void EnemyManager::Pause()
{
	m_pauseStart = GetTickCount64();
}

void EnemyManager::Resume()
{
	// 멈춰 있던 시간만큼 난이도 기준과 다음 스폰 시각을 함께 뒤로 민다.
	// (난이도에서 정지 시간을 제외하고, 재개 즉시 적이 몰려나오지 않게 한다.)
	ULONGLONG paused = GetTickCount64() - m_pauseStart;
	m_startTime     += paused;
	m_nextSpawnTime += paused;
}

ULONGLONG EnemyManager::CurrentSpawnDelay(ULONGLONG cur) const
{
	// 경과 시간을 0~1로 정규화(ENEMY_SPAWN_DELAY_RAMP초에 도달하면 1로 고정)한 뒤
	// 초기값에서 하한까지 선형 보간한다.
	float elapsedSec = (cur - m_startTime) / 1000.f;
	float t = elapsedSec / ENEMY_SPAWN_DELAY_RAMP;
	if (t > 1.f) t = 1.f;

	float delaySec = ENEMY_SPAWN_DELAY + (ENEMY_SPAWN_DELAY_MIN - ENEMY_SPAWN_DELAY) * t;
	return (ULONGLONG)(delaySec * 1000.f);
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
	m_startTime = 0;
	m_spawnDelay = 0;
	m_nextSpawnTime = 0;
}

void EnemyManager::TrySpawnEnemyInRandomPos(const Vector2* playerPos, int groundLength)
{
    ULONGLONG cur = GetTickCount64();
    if (m_nextSpawnTime < cur)
    {
        // 다음 스폰 간격은 현재 경과 시간 기준으로 다시 계산해 점점 짧아지게 한다.
        m_nextSpawnTime = cur + CurrentSpawnDelay(cur);

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

void EnemyManager::KillAll()
{
	for (auto& enemy : m_enemys)
	{
		if (enemy != nullptr)
			enemy->Kill();   // Kill()이 enemy_death 사운드 재생 + 사망 처리.
	}
}
