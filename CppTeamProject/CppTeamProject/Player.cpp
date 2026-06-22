#include "Player.h"
#include "Enemy.h"
#include "Collider.h"
#include "ColliderManager.h"
#include "Console.h"
#include "Defines.h"
#include "SoundManager.h"
#include <cmath>
#include <cstdlib>

Player::Player(Vector2 pos)
	: Pawn(pos, Color::LIGHT_GREEN, "●", { SCREEN_WIDTH, SCREEN_HEIGHT }, ColliderTag::PLAYER)
{
	// 표시 속도 SPEED_MAX까지 낼 수 있도록 실제 최고 속도 = SPEED_MAX * MOVE_SCALE (적과 공유 스케일).
	m_rigidbody->SetMaxSpeed(SPEED_MAX * MOVE_SCALE);
}

void Player::Tick()
{
	// 무적 중이어도 땅에 닿아 있으면 조작 가능. 공중(넉백 비행) 중에는 입력을 받지 않는다.
	// (입력 처리가 IsGrounded() 블록 안에 있으므로 공중 차단은 자동으로 보장된다.)
	if (m_rigidbody->IsGrounded())
	{
		if (GetKey(VK_XBUTTON1))
		{
			m_rigidbody->SetFrozen(true);

			int wheel = GetMouseWheelChange();
			if (wheel != 0)
			{
				m_chargedWheel += wheel;        // 차징 누적(색 미리보기/화면 흔들림용)

				// 표시 속도 상한(SPEED_MAX)에 해당하는 휠 수로 제한 → 무한 증가 방지.
				const int maxCharge = static_cast<int>(SPEED_MAX / PLAYER_SPEED);
				if (m_chargedWheel >  maxCharge) m_chargedWheel =  maxCharge;
				if (m_chargedWheel < -maxCharge) m_chargedWheel = -maxCharge;

				SOUND->PlayRestart("wheel");    // 이전 소리 멈추고 다시 재생
			}

			Pawn::Tick();
			UpdateInvincibility();
			UpdateCameraShake();
			UpdateColor();
			return;
		}

		if (m_rigidbody->IsFrozen())
		{
			m_rigidbody->SetFrozen(false);

			// 제한된 차징량만큼 발사(표시 속도 → 실제 이동 속도).
			SetVelocity(m_chargedWheel * PLAYER_SPEED * MOVE_SCALE);

			m_chargedWheel = 0;   // 차징 종료: 누적 리셋(freeze 화면 흔들림 종료)
		}

		int rollWheel = GetMouseWheelChange();
		if (rollWheel != 0)
			SOUND->PlayRestart("wheel");    // 이전 소리 멈추고 다시 재생
		SetVelocity(rollWheel * PLAYER_SPEED * MOVE_SCALE);

		if (GetKey('a') || GetKey('A'))
			SetVelocity(-1 * PLAYER_KEYBOARD_SPEED * MOVE_SCALE);
		if (GetKey('d') || GetKey('D'))
			SetVelocity(1 * PLAYER_KEYBOARD_SPEED * MOVE_SCALE);
	}

	Pawn::Tick();
	UpdateInvincibility();
	UpdateCameraShake();
	UpdateColor();
}

float Player::ColorSpeed() const
{
	// 표시(논리) 속도. 적과 같은 스케일이라 표시 속도가 같으면 색도 같다.
	// 차징 중엔 누적 휠로 미리보기, 평상시엔 실제 속도를 MOVE_SCALE로 역환산.
	float logical;
	if (m_rigidbody->IsFrozen() && m_chargedWheel != 0)
		logical = std::fabs(static_cast<float>(m_chargedWheel)) * PLAYER_SPEED;
	else
		logical = std::fabs(m_rigidbody->GetVelocity()) / MOVE_SCALE;

	if (logical > SPEED_MAX)   // 색 단계 상한과 일치시킴(미리보기/실제 색 불일치 방지)
		logical = SPEED_MAX;
	return logical;
}

void Player::UpdateColor()
{
	// 적과 같은 색일 때 크리티컬이 나도록, 색 판정용 속도로 색을 정한다.
	m_renderColor = Enemy::ColorForSpeed(ColorSpeed());
}

void Player::OnCollision(Collider* other)
{
	// 이미 죽은 뒤에는 충돌을 처리하지 않는다(사망 후 크리티컬 등 효과음 방지).
	if (IsDead())
		return;

	ColliderTag tag = other->GetTag();

	if (tag == ColliderTag::ENEMY)
	{
		// 무적 중에는 적과 충돌하지 않는다.
		if (m_isInvincible)
			return;

		Enemy* enemy = static_cast<Enemy*>(other->GetOwner());
		if (!enemy || enemy->IsDying())   // 이미 죽는 중인 적은 재처리하지 않음
			return;

		// 스폰 직후 아직 땅에 닿지 않은(낙하 중) 적에게 맞으면 플레이어 즉사.
		if (!enemy->HasLanded())
		{
			ConsoleShakeRestore();
			ShakeConsoleWindow(DEATH_SHAKE_INTENSITY, DEATH_SHAKE_DURATION, DEATH_SHAKE_INTERVAL);
			SOUND->Play("player_death");
			SetDead();
			return;
		}

		ResolveEnemyHit(enemy, other);
		return;
	}

	if (tag == ColliderTag::SPIKE)
	{
		// 가시 사망: 마지막 카메라 쉐이킹 지점
		ConsoleShakeRestore();
		ShakeConsoleWindow(DEATH_SHAKE_INTENSITY, DEATH_SHAKE_DURATION, DEATH_SHAKE_INTERVAL);
		SOUND->Play("player_death");
		SetDead();
		return;
	}

	Pawn::OnCollision(other);   // TILE 등
}

void Player::ResolveEnemyHit(Enemy* enemy, Collider* enemyCol)
{
	const float playerVel = m_rigidbody->GetVelocity();

	// 플레이어와 적의 색(=속도 단계)이 같으면 크리티컬: 적 즉사, 반작용 0.
	if (Enemy::ColorForSpeed(ColorSpeed()) == enemy->GetColor())
	{
		m_rigidbody->SetVelocity(0.f);
		enemy->Kill();
		SOUND->Play("critical");
		return;
	}

	// 색이 다르면 플레이어가 진행 반대 방향으로 튕겨 나간다(적은 살아남음).
	int travelDir = (playerVel > 0.f) ? 1 : (playerVel < 0.f) ? -1 : 0;
	if (travelDir == 0)
	{
		int enemyCenterX = (enemyCol->GetLeft() + enemyCol->GetRight()) / 2;
		travelDir = (m_pos.x < enemyCenterX) ? 1 : -1;
	}

	ApplyKnockback(-travelDir, KNOCKBACK_CONST);
	SOUND->Play("hit");            // 색 불일치(넉백)
}

void Player::ApplyKnockback(int dir, float knockMag)
{
	m_rigidbody->AddKnockback(dir * knockMag, -KNOCKBACK_Y);

	// 넉백 후 무적 시작
	m_isInvincible    = true;
	m_invincibleTimer = PLAYER_INVINCIBLE_FRAMES;

	// 카메라 리코일 예약: 넉백 방향으로 세기에 비례해 밀렸다가 원위치로 복귀.
	m_impactDir       = dir;
	m_impactIntensity = knockMag * IMPACT_SHAKE_SCALE;
	m_impactFrames    = IMPACT_SHAKE_FRAMES;
}

void Player::UpdateInvincibility()
{
	if (!m_isInvincible)
		return;

	if (--m_invincibleTimer > 0)
		return;

	// 무적 해제
	m_isInvincible = false;

	// 해제 순간에도 적과 끼여 있으면: 죽지 않고 일정한 힘으로 랜덤 방향 튕긴다.
	if (!ColliderManager::GetInst()->IsOverlappingTag(m_collider.get(), ColliderTag::ENEMY))
		return;

	int dir = (std::rand() % 2 == 0) ? 1 : -1;   // 랜덤 좌/우
	ApplyKnockback(dir, KNOCKBACK_CONST);        // 다시 무적 부여(끼임 반복 사망 방지)
	SOUND->Play("hit");
}

void Player::UpdateCameraShake()
{
	// 충격 리코일이 진행 중이면 속도 흔들림은 동시에 발생하지 않는다.
	// 넉백 방향으로 m_impactIntensity만큼 밀렸다가, 남은 프레임 비율로 0까지 감쇠하며 복귀.
	if (m_impactFrames > 0)
	{
		--m_impactFrames;
		float t = static_cast<float>(m_impactFrames) / IMPACT_SHAKE_FRAMES;
		float offsetX = m_impactDir * m_impactIntensity * t;
		ConsoleRecoil(offsetX, 0.f);
		return;
	}

	// 무적 상태에서는 속도에 따른 흔들림이 발생하지 않는다.
	if (m_isInvincible)
	{
		ConsoleShakeRestore();
		return;
	}

	// 차징(freeze) 중에는 실제 속도가 0이므로, 저장된(미적용) 속도에 비례해 흔든다.
	if (m_chargedWheel != 0)
	{
		float chargedSpeed = std::fabs(static_cast<float>(m_chargedWheel)) * PLAYER_SPEED;
		ConsoleShake(chargedSpeed * FREEZE_SHAKE_SCALE, USE_FERRIS_SHAKE);
		return;
	}

	// 평상시: 플레이어 속도를 콘솔창 움직임으로 표현
	float speed = std::fabs(m_rigidbody->GetVelocity());
	if (speed >= SHAKE_MIN_SPEED)
		ConsoleShake(speed * VELOCITY_SHAKE_SCALE, USE_FERRIS_SHAKE);
	else
		ConsoleShakeRestore();
}

void Player::Render() const
{
	// 무적 중에는 깜빡인다.
	if (m_isInvincible)
	{
		bool hide = ((m_invincibleTimer / PLAYER_BLINK_INTERVAL) % 2) == 1;
		if (hide)
		{
			RemovePrevPos();
			m_hasLastRender = false;
			return;
		}
	}

	Pawn::Render();
}
