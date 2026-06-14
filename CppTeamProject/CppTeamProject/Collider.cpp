#include "Collider.h"
#include "ColliderManager.h"

Collider::Collider(Vector2* pPos, int halfWidth, void* pOwner)
	: m_pPos(pPos)
	, m_halfWidth(halfWidth)
	, m_pOwner(pOwner)
	, m_onCollision(nullptr)
{
}

Collider::~Collider()
{
	ColliderManager::GetInst()->UnregisterCollider(this);
}

int Collider::GetLeft() const
{
	return m_pPos->x - m_halfWidth;
}

int Collider::GetRight() const
{
	return m_pPos->x + m_halfWidth;
}

int Collider::GetTop() const
{
	return m_pPos->y;
}

int Collider::GetBottom() const
{
	return m_pPos->y + 1; // height = 1
}

bool Collider::Overlaps(const Collider& other) const
{
	return GetLeft() <= other.GetRight() && GetRight() >= other.GetLeft()
		&& GetTop() <= other.GetBottom() && GetBottom() >= other.GetTop();
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