#pragma once
#include "Vector2.h"
#include "Collider.h"
#include "Console.h"
#include <memory>

// 중력 없이 수평 직진하는 총알. 적에 맞으면 적을 죽이고, 벽/화면밖이면 소멸.
class Bullet
{
public:
	Bullet(Vector2 pos, int dir);

	void Tick();
	void Render() const;
	bool IsDead() const { return m_isDead; }
	bool DidKillEnemy() const { return m_killedEnemy; }   // 이 총알이 이번에 적을 처치했는지(타격감 진동용).

	void RemovePrevPos() const;   // 제거 직전 잔상 지우기.

private:
	void OnCollision(Collider* other);

	Vector2 m_pos;
	int     m_dir;                 // +1 오른쪽, -1 왼쪽.
	float   m_accumX = 0.f;        // 정수 셀 이동을 위한 소수 누적.
	bool    m_isDead = false;
	bool    m_killedEnemy = false; // 적을 처치하며 소멸했는지(벽/화면밖 소멸과 구분).

	std::unique_ptr<Collider> m_collider;

	mutable Vector2 m_lastRenderPos{ 0, 0 };
	mutable bool    m_hasLastRender = false;
};
