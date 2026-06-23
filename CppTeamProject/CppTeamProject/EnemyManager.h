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
	void KillAll();   // 필드의 모든 적을 즉사시킨다(모두죽이기 아이템).

	// 설정창(일시정지) 동안 난이도 경과·스폰 타이밍을 멈췄다 재개한다.
	void Pause();
	void Resume();
private:
	// 경과 시간(cur - m_startTime)에 따라 줄어드는 현재 스폰 간격(ms)을 계산한다.
	ULONGLONG CurrentSpawnDelay(ULONGLONG cur) const;
private:
	static EnemyManager* m_inst;
	std::vector<std::unique_ptr<Enemy>> m_enemys;
	ULONGLONG m_startTime = 0;
	ULONGLONG m_pauseStart = 0;
	ULONGLONG m_spawnDelay = 0;
	ULONGLONG m_nextSpawnTime = 0;
};
