#pragma once
#include "Vector2.h"
#include "Defines.h"

class Rigidbody
{
public:
	Rigidbody(Vector2* pPos, float friction = 0.75f, float maxSpeed = 3.0f, float gravityScale = 1.0f);

	void Tick(float deltaTime = 1.0f / 60.0f);

	void AddForce(float force);
	void AddKnockback(float forceX, float forceY);   // 수평/수직 동시 임펄스 + 넉백 윈도우 시작
	void SetVelocity(float velocity);

	// 부스트(차징 발사) 윈도우 시작: 이 동안은 마찰/정지 임계값을 건너뛰어 속도가 유지된다(멀리 미끄러짐).
	void StartBoost(int frames) { m_boostFrames = frames; }

	bool IsKnockback() const { return m_knockbackFrames > 0; }

	void SetGrounded(bool grounded);

	// 프리즈: 켜져 있는 동안 수평 이동/마찰만 멈추고 수평 속도는 보존. 중력은 계속 적용됨.
	void SetFrozen(bool frozen) { m_frozen = frozen; }
	bool IsFrozen() const { return m_frozen; }

	void SetFriction(float friction) { m_friction = friction; }
	void SetMaxSpeed(float maxSpeed) { m_maxSpeed = maxSpeed; }

	float GetVelocity()  const { return m_velocity; }
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
	bool m_frozen;

	int  m_knockbackFrames = 0;   // >0 동안 넉백 진행 중(maxSpeed 클램프/deadzone 무시)
	int  m_boostFrames     = 0;   // >0 동안 부스트(차징 발사) 진행 중(마찰/정지 임계값 무시)
};
