#include "Pawn.h"
#include "Console.h"
#include "ColliderManager.h"
#include "Collider.h"
#include "Defines.h"
#include <cmath>
#include <algorithm>

Pawn::Pawn(Vector2 _pos, Color renderColor, std::string renderIcon, Vector2 deadZone, ColliderTag tag)
	: Actor(_pos, renderColor, renderIcon)
	, m_deadZone(deadZone)
	, m_rigidbody(std::make_unique<Rigidbody>(&m_pos))
	, m_collider(std::make_unique<Collider>(&m_pos, 1, 1, this, tag))  // 폭 1셀 × 높이 1셀, 좌상단 = m_pos
{
	m_collider->SetOnCollision([this](Collider* collider){this->OnCollision(collider); });

	ColliderManager::GetInst()->RegisterCollider(m_collider.get());
}

Pawn::~Pawn()
{
	ColliderManager::GetInst()->UnregisterCollider(m_collider.get());
}

void Pawn::SetVelocity(float velocityX)
{
	m_rigidbody->AddForce(velocityX);
}

bool Pawn::IsLeaveDeadZone()
{
	return (m_pos.x > SCREEN_WIDTH - 2 || m_pos.x < 1
			|| m_pos.y > SCREEN_HEIGHT || m_pos.y < 0);
}

void Pawn::RemovePrevPos() const
{
	if (!m_hasLastRender)
		return;

	SetColor();
	GotoXY(m_lastRenderPos.x, m_lastRenderPos.y);
	cout << "  ";
}

void Pawn::OnCollision(Collider* other)
{
	ColliderTag tag = other->GetTag();

	if (tag == ColliderTag::SPIKE)
	{
		m_isDead = true;
		return;
	}

	if (tag == ColliderTag::TILE)
	{
		int ml = m_collider->GetLeft();
		int mr = m_collider->GetRight();
		int mt = m_collider->GetTop();
		int mb = m_collider->GetBottom();

		int ol = other->GetLeft();
		int orr = other->GetRight();
		int ot = other->GetTop();
		int ob = other->GetBottom();

		int overlapX = std::min(mr, orr) - std::max(ml, ol);
		int overlapY = std::min(mb, ob) - std::max(mt, ot);
		if (overlapX < 0 || overlapY < 0)
			return;

		if (overlapY <= overlapX)
		{
			int mCenterY = (mt + mb) / 2;
			int oCenterY = (ot + ob) / 2;
			if (mCenterY <= oCenterY)
			{
				m_pos.y = ot - m_collider->GetHeight();
				m_rigidbody->SetGrounded(true);
			}
			else if (overlapY > 0)
			{
				m_pos.y = ob;
			}
		}
		else if (overlapX > 0)
		{
			int pCenterX = (ml + mr) / 2;
			int oCenterX = (ol + orr) / 2;
			if (pCenterX <= oCenterX)
				m_pos.x = ol - m_collider->GetWidth();
			else
				m_pos.x = orr;
		}
	}
	else if (tag == ColliderTag::ENEMY || tag == ColliderTag::PLAYER)
	{
		int otherCenterX = (other->GetLeft() + other->GetRight()) / 2;
		int dir = (m_pos.x < otherCenterX) ? -1 : 1;
		float speed = std::abs(m_rigidbody->GetVelocity());
		float knockX = std::max(speed, KNOCKBACK_MIN) * KNOCKBACK_SCALE;
		m_rigidbody->SetGrounded(false);
		m_rigidbody->AddForce(dir * knockX);

		if (tag == ColliderTag::ENEMY && m_collider->GetTag() == ColliderTag::PLAYER)
		{
			if (m_rigidbody->IsFrozen())
				m_rigidbody->SetFrozen(false);
			m_rigidbody->AddForceY(-KNOCKBACK_Y * knockX);
		}
	}
}

void Pawn::Tick()
{
	m_rigidbody->SetGrounded(false);
	m_rigidbody->Tick();
}

void Pawn::Render() const
{
	RemovePrevPos();

	SetColor(m_renderColor);
	GotoXY(m_pos.x, m_pos.y);
	cout << m_renderIcon;

	m_lastRenderPos = m_pos;
	m_hasLastRender = true;
}
