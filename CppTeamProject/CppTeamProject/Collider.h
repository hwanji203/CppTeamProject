#pragma once
#include "Vector2.h"
#include <functional>

enum class ColliderTag { TILE, PLAYER, ENEMY, SPIKE, ITEM, BULLET };

class Collider
{
public:
	// pPos는 박스의 좌상단(스크린 좌표). width/height는 차지하는 셀 수.
	Collider(Vector2* pPos, int width, int height = 1, void* pOwner = nullptr, ColliderTag tag = ColliderTag::PLAYER);
	~Collider();

	int  GetLeft()  const;
	int  GetRight() const;
	int  GetTop()    const;
	int  GetBottom() const;
	bool Overlaps(const Collider& other) const;

	void SetOnCollision(std::function<void(Collider*)> callback);
	void InvokeCollision(Collider* other);

	void* GetOwner()    const { return m_pOwner; }
	int          GetWidth()  const { return m_width; }
	int          GetHeight() const { return m_height; }
	ColliderTag  GetTag()       const { return m_tag; }

private:
	Vector2* m_pPos;
	int         m_width;
	int         m_height;
	void* m_pOwner;
	ColliderTag m_tag;

	std::function<void(Collider*)> m_onCollision;
};
