#include "Rigidbody.h"
#include "Defines.h"
#include <algorithm>
#include <cmath>

Rigidbody::Rigidbody(Position* pPos, float friction, float maxSpeed)
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

	m_velocity *= m_friction;

	if (std::fabs(m_velocity) < 0.01f)
	{
		m_velocity = 0.f;
		m_accumX   = 0.f;
		return;
	}

	m_velocity = std::clamp(m_velocity, -m_maxSpeed, m_maxSpeed);

	m_accumX += m_velocity * deltaTime * FRAME;
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
