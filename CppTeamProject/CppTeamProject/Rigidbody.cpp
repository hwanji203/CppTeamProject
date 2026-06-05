#include "Rigidbody.h"
#include "Defines.h"
#include <algorithm>
#include <cmath>

Rigidbody::Rigidbody(Vector2* pPos, float friction, float maxSpeed)
	: m_pPos(pPos)
	, m_velocity(0.f)
	, m_friction(friction)
	, m_maxSpeed(maxSpeed)
	, m_accumX(0.f)
{
}

void Rigidbody::Tick(float deltaTime)
{
	if (!m_pPos)
		return;

	// 마찰 적용 (속도를 0 방향으로 감쇠)
	m_velocity *= m_friction;

	// 속도가 매우 작으면 완전히 멈춤
	if (std::fabs(m_velocity) < 0.01f)
	{
		m_velocity = 0.f;
		m_accumX   = 0.f;
		return;
	}

	// 최대 속도 clamp
	m_velocity = std::clamp(m_velocity, -m_maxSpeed, m_maxSpeed);

	// 소수점 누적 후 정수 칸 수만 포지션에 반영
	m_accumX += m_velocity * deltaTime * FRAME; // FRAME 배율로 콘솔 칸 단위 조정
	int steps = static_cast<int>(m_accumX);
	if (steps != 0)
	{
		m_pPos->x += steps;
		m_accumX  -= static_cast<float>(steps);
	}
}

void Rigidbody::AddForce(float force)
{
	m_velocity += force;
	m_velocity = std::clamp(m_velocity, -m_maxSpeed, m_maxSpeed);
}

void Rigidbody::SetVelocity(float velocity)
{
	m_velocity = std::clamp(velocity, -m_maxSpeed, m_maxSpeed);
}

void Rigidbody::StopMovement()
{
	m_velocity = 0.f;
	m_accumX   = 0.f;
}
