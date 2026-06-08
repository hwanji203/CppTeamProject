#pragma once
#include "Utils.h"
#include "Windows.h"
#include<vector>
#include "Enemy.h"

class EnemyManager
{
private:
	EnemyManager(ULONGLONG spawnDelay = 100);
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

	void Init(float spawnDelay);
public:
	void TrySpawnEnemyInRandomPos();
private:
	static EnemyManager* m_inst;
	std::vector<std::unique_ptr<Enemy>> m_enemys;
	ULONGLONG m_spawnDelay;
	ULONGLONG m_nextSpawnTime;
};
