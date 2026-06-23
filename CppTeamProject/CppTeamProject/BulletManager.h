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
	void Update();
	void Render();
	void Clear();

private:
	static BulletManager* m_inst;
	std::vector<std::unique_ptr<Bullet>> m_bullets;
};
