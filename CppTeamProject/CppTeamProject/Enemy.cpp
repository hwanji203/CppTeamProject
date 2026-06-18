#include "Enemy.h"
#include "Console.h"
#include "ColliderManager.h"
#include "Defines.h"

Enemy::Enemy(const Vector2* playerPos, int power, Vector2 pos)
	: Pawn(pos, Color::LIGHT_RED, "★", { SCREEN_WIDTH, SCREEN_HEIGHT }, ColliderTag::ENEMY),
	m_power(power),
	m_playerPos(playerPos)
{
	m_rigidbody->SetMaxSpeed(0.25f);
}

void Enemy::Tick()
{
	if (m_rigidbody->IsGrounded())
	{
		int dir = 0;

		if ((m_playerPos->x - m_pos.x) > 0)
			dir = 1;
		else if ((m_playerPos->x - m_pos.x) < 0)
			dir = -1;

		//m_rigidbody->AddForce(dir * ENEMY_SPEED);
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
