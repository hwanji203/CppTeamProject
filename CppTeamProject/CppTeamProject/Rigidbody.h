#pragma once
#include "Vector2.h"

class Rigidbody
{
public:
	Rigidbody(Vector2* pPos, float friction = 0.85f, float maxSpeed = 20.0f, float gravityScale = 1.0f);

	void Tick(float deltaTime = 1.0f / 60.0f);

	void AddForce(float force);
	void SetVelocity(float velocity);

	void SetGrounded(bool grounded);
	void SetGravityScale(float scale) { m_gravityScale = scale; }

	float GetVelocity()  const { return m_velocity; }
	float GetVelocityY() const { return m_velocityY; }
	bool  IsMoving()     const { return m_velocity > 0.01f || m_velocity < -0.01f; }
	bool  IsGrounded()   const { return m_isGrounded; }

private:
	Vector2* m_pPos;

	float m_velocity;
	float m_velocityY;
	float m_friction;
	float m_maxSpeed;
	float m_gravityScale;

	float m_accumX;
	float m_accumY;

	bool m_isGrounded;
};
