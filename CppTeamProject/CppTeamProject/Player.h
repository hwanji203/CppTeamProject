#pragma once
#include "Pawn.h"

class Enemy;

// 입력(마우스 휠)으로 조작되는 플레이어.
// 입력 처리를 Pawn에서 분리해, Enemy는 더 이상 입력을 받지 않는다.
class Player : public Pawn
{
public:
	Player(Vector2 pos = { 0, 0 });

	virtual void Tick() override;
	virtual void Render() const override;

private:
	virtual void OnCollision(Collider* other) override;

	// 적과 충돌 시 색 비교 -> 같은 색이면 크리티컬, 아니면 넉백
	void ResolveEnemyHit(Enemy* enemy, Collider* enemyCol);
	void UpdateColor();           // 현재 속도에 따라 플레이어 색(=속도 단계) 갱신
	float ColorSpeed() const;     // 색 판정용 표시(논리) 속도 = 실제 속도 / MOVE_SCALE
	// dir 방향(±1) + 위로 넉백 적용 + 무적/카메라 리코일 예약(공통 처리)
	void ApplyKnockback(int dir, float knockMag);
	void UpdateInvincibility();   // 무적 카운트다운 및 해제 처리
	void UpdateCameraShake();     // 속도/충격/차징 기반 카메라 쉐이킹 구동

	// 무적 상태(넉백 후 깜빡임 + 입력/충돌 차단)
	bool m_isInvincible    = false;
	int  m_invincibleTimer = 0;

	// 충격 카메라 리코일(넉백 방향으로 밀렸다 복귀)
	float m_impactIntensity = 0.f;   // 최대 밀림 픽셀
	int   m_impactFrames    = 0;     // 남은 리코일 프레임
	int   m_impactDir       = 0;     // 밀림 방향(±1)

	// 차징(freeze) 상태: 누적된(아직 적용 안 된) 저장 속도 = 휠 합 × PLAYER_SPEED
	int m_chargedWheel = 0;   // 현재 차징에서 누적된 휠 합(freeze 화면 흔들림용)
};
