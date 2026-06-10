#pragma once
#include "Vector2.h"
#include <functional>

class Collider
{
public:
	Collider(Vector2* pPos, int halfWidth, void* pOwner = nullptr);

	int  GetLeft()  const;
	int  GetRight() const;
	int  GetTop()    const;
	int  GetBottom() const;
	bool Overlaps(const Collider& other) const;

	void SetOnCollision(std::function<void(Collider*)> callback);
	void InvokeCollision(Collider* other);

	void* GetOwner() const { return m_pOwner; }
	int   GetHalfWidth() const { return m_halfWidth; }

private:
	Vector2* m_pPos;
	int       m_halfWidth;
	void*     m_pOwner; 

	std::function<void(Collider*)> m_onCollision;
};
