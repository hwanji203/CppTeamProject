#include "ItemManager.h"
#include "Defines.h"
#include <cstdlib>

ItemManager* ItemManager::m_inst = nullptr;

int ItemManager::RandomSpawnDelayFrames() const
{
	// [MIN, MAX]초 사이 랜덤 -> 프레임. FRAME=60.
	float span = ITEM_SPAWN_DELAY_MAX - ITEM_SPAWN_DELAY_MIN;
	float sec  = ITEM_SPAWN_DELAY_MIN + (std::rand() / (float)RAND_MAX) * span;
	return (int)(sec * FRAME);
}

void ItemManager::Init()
{
	m_items.clear();
	m_spawnTimer = RandomSpawnDelayFrames();
}

void ItemManager::TrySpawnItem(int groundLength)
{
	// 프레임 기반 카운트다운. 오버레이 중에는 이 함수가 안 불려 자동 정지된다.
	if (--m_spawnTimer > 0)
		return;

	m_spawnTimer = RandomSpawnDelayFrames();

	// 타입은 4종 균등 랜덤.
	ItemType type = (ItemType)(std::rand() % (int)ItemType::COUNT);

	// x는 적 스폰과 동일하게 지면 좌우 전체에서 랜덤(화면 중앙 +- groundLength).
	int span   = groundLength * 2;
	int spawnX = SCREEN_WIDTH / 2 - groundLength + (std::rand() % span);

	m_items.push_back(std::make_unique<Item>(type, Vector2{ spawnX, 0 }));
}

void ItemManager::Update()
{
	for (auto it = m_items.begin(); it != m_items.end();)
	{
		if (*it != nullptr)
		{
			(*it)->Tick();

			if ((*it)->IsLeaveDeadZone() || (*it)->IsDead())
			{
				(*it)->RemovePrevPos();
				it = m_items.erase(it);
				continue;
			}
		}
		++it;
	}
}

void ItemManager::Render()
{
	for (auto& item : m_items)
	{
		if (item != nullptr)
			item->Render();
	}
}

void ItemManager::Clear()
{
	m_items.clear();
	m_spawnTimer = 0;
}
