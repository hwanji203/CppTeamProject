#include "Enemy.h"
#include "Console.h"
#include "ColliderManager.h"
#include "Defines.h"

Enemy::Enemy(const Vector2* playerPos, int power, Vector2 pos)
	: Pawn(pos, Color::RED, "бс", { SCREEN_WIDTH, SCREEN_HEIGHT }, ColliderTag::ENEMY),
	m_power(power),
	m_playerPos(playerPos),
	m_isSetVelocity(false)
{
	m_rigidbody->SetFriction(1);
}

void Enemy::Tick()
{
	if (m_isSetVelocity == false)
	{
		if (m_rigidbody->IsGrounded())
		{
			int dir = 0;
			
			if ((m_playerPos->x - m_pos.x) > 0)
				dir = 1;
			else if ((m_playerPos->x - m_pos.x) < 0)
				dir = -1;

			m_rigidbody->SetVelocity(dir * ENEMY_SPEED);
			m_isSetVelocity = true;
		}
	}

	Pawn::Tick();
}

void Enemy::Render() const
{
	Pawn::Render();

	GotoXY(m_pos.x, m_pos.y - 1);
	cout << m_power;
}

void Enemy::RemovePrevPos() const
{
	Pawn::RemovePrevPos();
	if (!m_hasLastRender)
		return;

	SetColor();
	GotoXY(m_lastRenderPos.x, m_lastRenderPos.y - 1);
	cout << " ";
}
