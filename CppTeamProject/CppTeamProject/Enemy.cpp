#include "Enemy.h"
#include "Console.h"
#include "ColliderManager.h"
#include "Defines.h"

Enemy::Enemy(int power, Vector2 pos)
	: Pawn(pos, "q", { SCREEN_WIDTH, SCREEN_HEIGHT }, ColliderTag::ENEMY),
	m_power(power)
{
}

void Enemy::Tick()
{
	Pawn::Tick();
	m_rigidbody->SetVelocity(1);
}

void Enemy::Render() const
{
	Pawn::Render();
}
