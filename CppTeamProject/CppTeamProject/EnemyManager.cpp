#include "EnemyManager.h"
#include "Defines.h"

EnemyManager::EnemyManager(ULONGLONG spawnDelay)
	:m_spawnDelay(spawnDelay)
{
	m_nextSpawnTime = GetTickCount64() + m_spawnDelay;
}

void EnemyManager::Init(float spawnDelay)
{
	m_spawnDelay = spawnDelay;
	m_nextSpawnTime = GetTickCount64() + m_spawnDelay;
}

void EnemyManager::TrySpawnEnemyInRandomPos()
{
	ULONGLONG cur = GetTickCount64();
	if (m_nextSpawnTime < cur)
	{
		m_nextSpawnTime = cur + m_spawnDelay;
		Vector2 startPos = { std::rand() % SCREEN_WIDTH };
		auto enemy = std::make_unique<Enemy>();
		m_enemys.push_back(move(enemy));
	}
}