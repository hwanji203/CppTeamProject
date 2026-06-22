#pragma once
#include "Rigidbody.h"
#include "Collider.h"
#include "Pawn.h"
#include <memory>

class Enemy : public Pawn
{
public:
	// speed = 랜덤 추적 속도. 이 속도 하나로 색(4단계)이 결정된다.
	Enemy(const Vector2* playerPos, float speed, Vector2 pos = { 0, 0 });

	virtual void Tick() override;

	float GetSpeed() const { return m_speed; }   // 색 판정에 쓰는 고정 속도
	Color GetColor() const { return m_renderColor; }

	void Kill();                       // 크리티컬(같은 색) 즉사
	bool IsDying() const { return m_isDying; }
	bool HasLanded() const { return m_hasLanded; }   // 스폰 후 한 번이라도 착지했는가

	// 속도 → 4단계 색. 0~1 초록, 1~2 노랑, 2~3 주황(어두운 노랑), 3 이상 빨강.
	// 플레이어·적 공통으로 사용해 "같은 색이면 크리티컬"을 판정한다.
	static Color ColorForSpeed(float speed);

private:
	virtual void OnCollision(Collider* other) override;

	float m_speed;                     // 추적 속도(= 색 단계)
	const Vector2* m_playerPos;

	bool m_isDying = false;
	int  m_deathTimer = 0;
	bool m_hasLanded = false;          // 스폰 직후 낙하 중이면 false, 첫 착지 후 true
};
