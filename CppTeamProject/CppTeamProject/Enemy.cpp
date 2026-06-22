#include "Enemy.h"
#include "Console.h"
#include "ColliderManager.h"
#include "Collider.h"
#include "Defines.h"
#include "SoundManager.h"

Color Enemy::ColorForSpeed(float speed)
{
	// 0~1 초록, 1~2 노랑, 2~3 주황(콘솔에 주황이 없어 어두운 노랑으로 근사), 3 이상 빨강.
	if (speed == 0) return Color::LIGHT_GREEN;
	if (speed < 1.0f) return Color::LIGHT_YELLOW;
	if (speed < 2.0f) return Color::YELLOW;
	return Color::LIGHT_RED;
}

Enemy::Enemy(const Vector2* playerPos, float speed, Vector2 pos)
	: Pawn(pos, ColorForSpeed(speed), "■", { SCREEN_WIDTH, SCREEN_HEIGHT }, ColliderTag::ENEMY),
	m_speed(speed),
	m_playerPos(playerPos)
{
	// 색은 표시 속도로 정하되, 실제 이동은 공유 배수로 낮춘 속도를 쓴다(표시 값 유지).
	m_rigidbody->SetMaxSpeed(speed * MOVE_SCALE);
}

void Enemy::Kill()
{
	// 크리티컬(같은 색) 즉사: 잠깐 빨갛게 깜빡인 뒤 사망.
	if (m_isDying)
		return;

	m_isDying     = true;
	m_deathTimer  = ENEMY_DEATH_FLASH_FRAMES;
	m_renderColor = Color::RED;
	m_rigidbody->SetVelocity(0.f);   // 죽는 순간 관성 제거(가던 방향으로 미끄러지는 버그 방지)
	SOUND->Play("enemy_death");
}

void Enemy::OnCollision(Collider* other)
{
	// 플레이어와의 충돌은 Player가 전담(넉백/크리티컬)하므로 무시.
	if (other->GetTag() == ColliderTag::PLAYER)
		return;

	Pawn::OnCollision(other);       // TILE/SPIKE 처리
}

void Enemy::Tick()
{
	if (m_isDying)
	{
		// 빨강 플래시 동안 제자리에 멈춰 있다가 죽는다(수평 관성 0 유지).
		m_rigidbody->SetVelocity(0.f);

		if (--m_deathTimer <= 0)
			SetDead();

		Pawn::Tick();
		return;
	}

	if (m_rigidbody->IsGrounded())
	{
		m_hasLanded = true;   // 첫 착지 이후로는 '공중 즉사' 대상이 아니다.

		int dir = 0;

		if ((m_playerPos->x - m_pos.x) > 0)
			dir = 1;
		else if ((m_playerPos->x - m_pos.x) < 0)
			dir = -1;

		m_rigidbody->AddForce(dir * m_speed * MOVE_SCALE);   // 낮춘 속도로 플레이어 추적
	}

	Pawn::Tick();
}
