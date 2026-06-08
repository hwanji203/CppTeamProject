#include "Rigidbody.h"
#include "Defines.h"
#include <algorithm>
#include <cmath>

static constexpr float GRAVITY        = 0.02f;
static constexpr float MAX_FALL_SPEED = 4.0f;

Rigidbody::Rigidbody(Vector2* pPos, float friction, float maxSpeed, float gravityScale)
	: m_pPos(pPos)
	, m_velocity(0.f)
	, m_velocityY(0.f)
	, m_friction(friction)
	, m_maxSpeed(maxSpeed)
	, m_gravityScale(gravityScale)
	, m_accumX(0.f)
	, m_accumY(0.f)
	, m_isGrounded(false)
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
	}
	else
	{
		m_velocity = std::clamp(m_velocity, -m_maxSpeed, m_maxSpeed);

		m_accumX += m_velocity * deltaTime * FRAME;
		int stepsX = static_cast<int>(m_accumX);
		if (stepsX != 0)
		{
			m_pPos->x += stepsX;
			m_accumX  -= static_cast<float>(stepsX);
		}
	}

	if (!m_isGrounded)
	{
		m_velocityY += GRAVITY * m_gravityScale;
		m_velocityY  = std::min(m_velocityY, MAX_FALL_SPEED);
	}

	m_accumY += m_velocityY * deltaTime * FRAME;
	int stepsY = static_cast<int>(m_accumY);
	if (stepsY != 0)
	{
		m_pPos->y += stepsY;
		m_accumY  -= static_cast<float>(stepsY);
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

void Rigidbody::SetGrounded(bool grounded)
{
	m_isGrounded = grounded;
	if (grounded)
	{
		m_velocityY = 0.f;
		m_accumY    = 0.f;
	}
}
