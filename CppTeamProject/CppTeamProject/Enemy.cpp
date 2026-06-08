#include "Enemy.h"
#include "Console.h"
#include "ColliderManager.h"

Enemy::Enemy(Vector2 pos, int power)
	: Pawn(pos),
	m_power(power)
{
}

void Enemy::Tick()
{
	m_rigidbody->Tick();
}

void Enemy::Render() const
{
	SetColor(Color::GRAY);
	GotoXY(m_pos.x, m_pos.y - 1);
	cout << m_power;

	GotoXY(m_pos.x, m_pos.y);
	cout << "ขอ";
}
