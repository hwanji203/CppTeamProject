#include "Collider.h"
#include "ColliderManager.h"

Collider::Collider(Vector2* pPos, int width, int height, void* pOwner, ColliderTag tag)
	: m_pPos(pPos)
	, m_width(width)
	, m_height(height)
	, m_pOwner(pOwner)
	, m_tag(tag)
	, m_onCollision(nullptr)
{
}

Collider::~Collider()
{
	ColliderManager::GetInst()->UnregisterCollider(this);
}

int Collider::GetLeft() const
{
	return m_pPos->x;
}

int Collider::GetRight() const
{
	// 마지막 셀 다음의 경계선(half-open). [GetLeft, GetRight) 사이 셀을 차지.
	return m_pPos->x + m_width;
}

int Collider::GetTop() const
{
	return m_pPos->y;
}

int Collider::GetBottom() const
{
	return m_pPos->y + m_height;
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