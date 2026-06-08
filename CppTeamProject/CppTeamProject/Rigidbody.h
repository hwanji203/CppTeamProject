#pragma once
#include "Vector2.h"

class Rigidbody
{
public:
	Rigidbody(Vector2* pPos, float friction = 0.85f, float maxSpeed = 20.0f);

	void Tick(float deltaTime = 1.0f / 60.0f);
	void AddForce(float force);
	void SetVelocity(float velocity);
	void StopMovement();

	float GetVelocity() const { return m_velocity; }
	bool IsMoving()     const { return m_velocity > 0.01f || m_velocity < -0.01f; }

private:
	Vector2* m_pPos;

	float m_velocity;
	float m_friction;
	float m_maxSpeed;

	float m_accumX;
};
