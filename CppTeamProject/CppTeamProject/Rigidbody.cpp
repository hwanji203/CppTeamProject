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
	, m_frozen(false)
{
}

void Rigidbody::Tick(float deltaTime)
{
	if (!m_pPos)
		return;

	// 프리즈 중에는 수평 이동/마찰만 멈추고 속도는 보존. (중력은 계속 적용)
	if (!m_frozen)
	{
		const bool knock = (m_knockbackFrames > 0);
		const bool boost = (m_boostFrames > 0);   // 차징 발사: 마찰/정지 임계값을 잠시 멈춰 멀리 미끄러진다.

		// 넉백/부스트 중에는 정지 임계값 처리를 건너뛴다(속도가 깎이지 않게).
		// 임계값을 키보드 한 프레임 힘보다 작게 둬야 작은 입력도 쌓여서 움직인다.
		if (!knock && !boost && m_velocity < MOVE_STOP_THRESHOLD && m_velocity > -MOVE_STOP_THRESHOLD)
			m_velocity = 0;

		if (!boost)
			m_velocity *= m_friction;   // 부스트 동안은 마찰 미적용(발사 속도 유지)

		if (!knock && !boost && std::fabs(m_velocity) < 0.0001f)
		{
			m_velocity = 0.f;
			m_accumX   = 0.f;
		}
		else
		{
			// 넉백 중에는 maxSpeed가 아닌 넉백 상한으로 클램프(적의 느린 속도에 묶이지 않음).
			const float cap = knock ? KNOCKBACK_MAX : m_maxSpeed;
			m_velocity = std::clamp(m_velocity, -cap, cap);

			m_accumX += m_velocity * deltaTime * FRAME;
			int stepsX = static_cast<int>(m_accumX);
			if (stepsX != 0)
			{
				m_pPos->x += stepsX;
				m_accumX  -= static_cast<float>(stepsX);
			}
		}

		if (m_boostFrames > 0)
			--m_boostFrames;
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

	if (m_knockbackFrames > 0)
		--m_knockbackFrames;
}

void Rigidbody::AddForce(float force)
{
	m_velocity += force;
	m_velocity = std::clamp(m_velocity, -m_maxSpeed, m_maxSpeed);
}

void Rigidbody::AddKnockback(float forceX, float forceY)
{
	m_velocity += forceX;
	m_velocity = std::clamp(m_velocity, -KNOCKBACK_MAX, KNOCKBACK_MAX);

	m_velocityY += forceY;
	m_velocityY = std::clamp(m_velocityY, -MAX_FALL_SPEED, MAX_FALL_SPEED);

	m_isGrounded      = false;
	m_knockbackFrames = KNOCKBACK_DURATION;
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
