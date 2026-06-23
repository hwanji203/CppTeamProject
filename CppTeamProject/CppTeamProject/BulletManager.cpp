#include "BulletManager.h"

BulletManager* BulletManager::m_inst = nullptr;

void BulletManager::Spawn(Vector2 pos, int dir)
{
	m_bullets.push_back(std::make_unique<Bullet>(pos, dir));
}

int BulletManager::Update()
{
	int kills = 0;
	for (auto it = m_bullets.begin(); it != m_bullets.end();)
	{
		if (*it != nullptr)
		{
			(*it)->Tick();

			if ((*it)->IsDead())
			{
				if ((*it)->DidKillEnemy())
					++kills;
				(*it)->RemovePrevPos();
				it = m_bullets.erase(it);
				continue;
			}
		}
		++it;
	}
	return kills;
}

void BulletManager::Render()
{
	for (auto& b : m_bullets)
	{
		if (b != nullptr)
			b->Render();
	}
}

void BulletManager::Clear()
{
	m_bullets.clear();
}
