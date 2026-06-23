#include "Bullet.h"
#include "ColliderManager.h"
#include "Enemy.h"
#include "Defines.h"
#include <algorithm>

Bullet::Bullet(Vector2 pos, int dir)
	: m_pos(pos)
	, m_dir(dir)
	, m_collider(std::make_unique<Collider>(&m_pos, 1, 1, this, ColliderTag::BULLET))
{
	m_collider->SetOnCollision([this](Collider* c) { this->OnCollision(c); });
	ColliderManager::GetInst()->RegisterCollider(m_collider.get());
}

void Bullet::OnCollision(Collider* other)
{
	ColliderTag tag = other->GetTag();

	if (tag == ColliderTag::ENEMY)
	{
		Enemy* enemy = static_cast<Enemy*>(other->GetOwner());
		if (enemy && !enemy->IsDying())
			enemy->Kill();
		m_isDead = true;
		return;
	}

	if (tag == ColliderTag::TILE)
	{
		// 경계만 닿는 바닥(겹침 0)은 스쳐 지나가고, 실제로 파고든 벽에만 소멸한다.
		int overlapX = std::min(m_collider->GetRight(), other->GetRight())
					 - std::max(m_collider->GetLeft(), other->GetLeft());
		int overlapY = std::min(m_collider->GetBottom(), other->GetBottom())
					 - std::max(m_collider->GetTop(), other->GetTop());
		if (overlapX > 0 && overlapY > 0)
			m_isDead = true;   // 벽 관통 X.
		return;
	}
	// PLAYER/ITEM/BULLET 등은 무시.
}

void Bullet::Tick()
{
	// 소수 속도를 누적해 정수 셀 단위로 이동.
	m_accumX += BULLET_SPEED;
	int steps = (int)m_accumX;
	if (steps != 0)
	{
		m_pos.x += m_dir * steps;
		m_accumX -= (float)steps;
	}

	// 화면 밖이면 소멸.
	if (m_pos.x < 0 || m_pos.x > SCREEN_WIDTH - 1)
		m_isDead = true;
}

void Bullet::RemovePrevPos() const
{
	if (!m_hasLastRender)
		return;

	SetColor();
	GotoXY(m_lastRenderPos.x, m_lastRenderPos.y);
	cout << "  ";
}

void Bullet::Render() const
{
	RemovePrevPos();

	SetColor(Color::CYAN);
	GotoXY(m_pos.x, m_pos.y);
	cout << "-";

	m_lastRenderPos = m_pos;
	m_hasLastRender = true;
}
