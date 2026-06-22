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

	int GetHp() const { return m_hp; }          // 좌상단 HP 바 표시용
	int GetMaxHp() const { return m_maxHp; }

private:
	virtual void OnCollision(Collider* other) override;

	// 적과 충돌 시 색 비교 -> 같은 색이면 크리티컬, 아니면 넉백
	void ResolveEnemyHit(Enemy* enemy, Collider* enemyCol);
	void UpdateColor();           // 현재 속도에 따라 플레이어 색(=속도 단계) 갱신
	float ColorSpeed() const;     // 색 판정용 표시(논리) 속도 = 실제 속도 / MOVE_SCALE
	// dir 방향(±1) + 위로 넉백 적용 + 무적/카메라 흔들림 예약(공통 처리)
	void ApplyKnockback(int dir, float knockMag);
	void UpdateInvincibility();   // 무적 카운트다운 및 해제 처리
	void UpdateCameraShake();     // 속도/이벤트/차징 기반 카메라 진동 구동
	void TriggerShake(float intensity, int frames);   // 이벤트 흔들림(방향 없는 진동) 시작

	void DrawSpeedText() const;   // 플레이어 위에 현재 표시 속도 수치
	void ClearSpeedText() const;  // 이전 프레임의 속도 수치 지우기

	// 무적 상태(넉백 후 깜빡임 + 입력/충돌 차단)
	bool m_isInvincible    = false;
	int  m_invincibleTimer = 0;

	// 이벤트 카메라 진동(넉백/적 처치 등): 세기만큼 흔들다 남은 프레임 비율로 감쇠. 방향 없음.
	float m_shakeIntensity = 0.f;    // 시작 흔들림 세기(px)
	int   m_shakeFrames    = 0;      // 남은 흔들림 프레임
	int   m_shakeFramesMax = 1;      // 감쇠 비율 계산용 시작 프레임 수

	// 플레이어 위 속도 수치: 이전에 그린 위치/길이를 기억해 다음 프레임에 지운다.
	mutable Vector2 m_lastSpeedPos{ 0, 0 };
	mutable int     m_lastSpeedLen = 0;
	mutable bool    m_hasLastSpeed = false;

	// 차징(freeze) 상태: 누적된(아직 적용 안 된) 저장 속도 = 휠 합 × PLAYER_SPEED
	int m_chargedWheel = 0;   // 현재 차징에서 누적된 휠 합(freeze 화면 흔들림용)

	// 체력: 색이 다른 적과 부딪히면 1 감소, 0이 되면 사망.
	int m_maxHp = PLAYER_MAX_HP;
	int m_hp    = PLAYER_MAX_HP;
};
