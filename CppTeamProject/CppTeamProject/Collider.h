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
	int       m_halfWidth;
	void*     m_pOwner; 

	std::function<void(Collider*)> m_onCollision;
};
