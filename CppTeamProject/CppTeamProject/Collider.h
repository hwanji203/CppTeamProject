#pragma once
#include "Position.h"
#include <functional>

class Collider
{
public:
	Collider(Position* pPos, int halfWidth, void* pOwner = nullptr);

	int  GetLeft()  const;
	int  GetRight() const;
	bool Overlaps(const Collider& other) const;

	void SetOnCollision(std::function<void(Collider*)> callback);
	void InvokeCollision(Collider* other);

	void* GetOwner() const { return m_pOwner; }
	int   GetHalfWidth() const { return m_halfWidth; }

private:
	Position* m_pPos;
	int       m_halfWidth; // 콜라이더 반폭 (좌우로 halfWidth 칸씩)
	void*     m_pOwner;    // 소유 Actor 식별용 (void* → 필요 시 static_cast)

	std::function<void(Collider*)> m_onCollision;
};
