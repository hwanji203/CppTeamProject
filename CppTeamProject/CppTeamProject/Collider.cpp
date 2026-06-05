#include "Collider.h"

Collider::Collider(Position* pPos, int halfWidth, void* pOwner)
	: m_pPos(pPos)
	, m_halfWidth(halfWidth)
	, m_pOwner(pOwner)
	, m_onCollision(nullptr)
{
}

int Collider::GetLeft() const
{
	return m_pPos->x - m_halfWidth;
}

int Collider::GetRight() const
{
	return m_pPos->x + m_halfWidth;
}

bool Collider::Overlaps(const Collider& other) const
{
	// 1D AABB: 두 선분이 겹치는지 확인
	return GetLeft() <= other.GetRight() && GetRight() >= other.GetLeft();
}

void Collider::SetOnCollision(std::function<void(Collider*)> callback)
{
	m_onCollision = callback;
}

void Collider::InvokeCollision(Collider* other)
{
	if (m_onCollision)
		m_onCollision(other);
}
