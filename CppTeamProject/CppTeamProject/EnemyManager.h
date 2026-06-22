#pragma once
#include "Utils.h"
#include "Windows.h"
#include<vector>
#include "Enemy.h"

class EnemyManager
{
private:
	EnemyManager() = default;
public:
	static EnemyManager* GetInst()
	{
		if (nullptr == m_inst)
			m_inst = new EnemyManager();
		return m_inst;
	}
	static void DestroyInst()
	{
		SAFE_DELETE(m_inst);
	}

	void Init();   // 스폰 간격은 Defines(ENEMY_SPAWN_DELAY)에서 가져온다.
	void Update();
	void Render();
	void Clear();
public:
	void TrySpawnEnemyInRandomPos(const Vector2* playerPos, int groundLength);
private:
	static EnemyManager* m_inst;
	std::vector<std::unique_ptr<Enemy>> m_enemys;
	ULONGLONG m_spawnDelay = 0;
	ULONGLONG m_nextSpawnTime = 0;
};
