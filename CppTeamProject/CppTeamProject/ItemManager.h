#pragma once
#include "Utils.h"
#include <vector>
#include <memory>
#include "Item.h"

class ItemManager
{
private:
	ItemManager() = default;
public:
	static ItemManager* GetInst()
	{
		if (nullptr == m_inst)
			m_inst = new ItemManager();
		return m_inst;
	}
	static void DestroyInst()
	{
		SAFE_DELETE(m_inst);
	}

	void Init();
	void Update();
	void Render();
	void Clear();

	// 매 프레임 호출. 내부 타이머가 0이 되면 랜덤 타입을 랜덤 위치에 스폰한다.
	void TrySpawnItem(int groundLength);

private:
	int RandomSpawnDelayFrames() const;   // [MIN,MAX]초를 프레임으로 환산한 랜덤 값.

	static ItemManager* m_inst;
	std::vector<std::unique_ptr<Item>> m_items;
	int m_spawnTimer = 0;   // 다음 스폰까지 남은 프레임.
};
