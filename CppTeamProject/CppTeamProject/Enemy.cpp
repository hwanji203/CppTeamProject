#include "Enemy.h"
#include "Console.h"
#include "ColliderManager.h"

Enemy::Enemy(int power, Vector2 pos)
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
