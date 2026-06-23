#pragma once
#include "Utils.h"
#include <vector>
#include <memory>
#include "Bullet.h"

class BulletManager
{
private:
	BulletManager() = default;
public:
	static BulletManager* GetInst()
	{
		if (nullptr == m_inst)
			m_inst = new BulletManager();
		return m_inst;
	}
	static void DestroyInst()
	{
		SAFE_DELETE(m_inst);
	}

	void Spawn(Vector2 pos, int dir);
	int  Update();   // 반환값: 이번 프레임에 총알이 처치한 적의 수(타격감 진동용).
	void Render();
	void Clear();

private:
	static BulletManager* m_inst;
	std::vector<std::unique_ptr<Bullet>> m_bullets;
};
