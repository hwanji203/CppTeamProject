#pragma once
#include "Position.h"

class Rigidbody
{
public:
	Rigidbody(Position* pPos, float friction = 0.85f, float maxSpeed = 20.0f);

	void Tick(float deltaTime = 1.0f / 60.0f);

	// 힘을 순간적으로 가함 (AddForce in Unity)
	void AddForce(float force);
	void SetVelocity(float velocity);
	void StopMovement();

	float GetVelocity() const { return m_velocity; }
	bool IsMoving()     const { return m_velocity > 0.01f || m_velocity < -0.01f; }

private:
	Position* m_pPos;

	float m_velocity;   // X 방향 속도
	float m_friction;   // 마찰 계수 (0 ~ 1, 1에 가까울수록 잘 안 멈춤)
	float m_maxSpeed;

	// int 그리드에서 소수점 이동을 누적해 픽셀 단위로 반영
	float m_accumX;
};
