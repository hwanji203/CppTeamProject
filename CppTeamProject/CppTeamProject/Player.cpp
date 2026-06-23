#include "Player.h"
#include "Enemy.h"
#include "Item.h"
#include "BulletManager.h"
#include "EnemyManager.h"
#include "Collider.h"
#include "ColliderManager.h"
#include "Console.h"
#include "Defines.h"
#include "SoundManager.h"
#include <cmath>
#include <cstdlib>
#include <cstdio>

Player::Player(Vector2 pos)
	: Pawn(pos, Color::LIGHT_GREEN, "●", { SCREEN_WIDTH, SCREEN_HEIGHT }, ColliderTag::PLAYER)
{
	// 표시 속도 SPEED_MAX까지 낼 수 있도록 실제 최고 속도 = SPEED_MAX * MOVE_SCALE (적과 공유 스케일).
	m_rigidbody->SetMaxSpeed(SPEED_MAX * MOVE_SCALE);

	// 마찰을 약간 줄여(=계수를 키워) 휠 한 칸당 더 멀리 미끄러지게 한다(이동량 ↑, 색은 그대로).
	m_rigidbody->SetFriction(PLAYER_FRICTION);
}

void Player::Tick()
{
	// 무적 중이어도 땅에 닿아 있으면 조작 가능. 공중(넉백 비행) 중에는 입력을 받지 않는다.
	// (입력 처리가 IsGrounded() 블록 안에 있으므로 공중 차단은 자동으로 보장된다.)
	if (m_rigidbody->IsGrounded())
	{
		// 뒤로 가기 버튼(마우스 XBUTTON1 또는 키보드 X)을 누른 채 휠을 돌리면 차징.
		if (GetBack())
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
			UpdateItemEffects();
			UpdateAppearance();
			return;
		}

		if (m_rigidbody->IsFrozen())
		{
			m_rigidbody->SetFrozen(false);

			// 모아둔 표시 속도로 발사하고, 잠시 마찰을 멈춰(부스트) 멀리 튀어 나가게 한다.
			m_rigidbody->SetVelocity(m_chargedWheel * PLAYER_SPEED * MOVE_SCALE);
			m_rigidbody->StartBoost(CHARGE_BOOST_FRAMES);

			m_chargedWheel = 0;   // 차징 종료: 누적 리셋
		}

		// 휠 한 칸을 굴리면 그만큼 가속하고, 마찰로 서서히 멈춘다(코스팅).
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
	UpdateItemEffects();
	UpdateAppearance();
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

void Player::UpdateAppearance()
{
	bool starOn = m_starTimer > 0;
	bool gunOn  = m_gunTimer  > 0;

	// 종료 임박(마지막 1초) 동안 효과 모습과 평상 모습을 번갈아 보여준다.
	int warnT = 0;
	if (starOn && m_starTimer <= STAR_BLINK_WARN_FRAMES)   warnT = m_starTimer;
	else if (gunOn && m_gunTimer <= GUN_BLINK_WARN_FRAMES) warnT = m_gunTimer;
	bool showNormal = (warnT > 0) && (((warnT / PLAYER_BLINK_INTERVAL) % 2) == 1);

	if ((!starOn && !gunOn) || showNormal)
	{
		// 평상 모습: 기본 아이콘 + 속도 색(크리티컬 판정용 색과 동일).
		m_renderIcon  = "●";
		m_renderColor = Enemy::ColorForSpeed(ColorSpeed());
	}
	else
	{
		// 효과 모습: 지속 효과 색(틴트, 가장 최근 것 우선) + 별이면 아이콘 ★.
		m_renderColor = Item::ColorForType(CurrentTintType());
		m_renderIcon  = starOn ? "★" : "●";
	}
}

void Player::OnCollision(Collider* other)
{
	// 이미 죽은 뒤에는 충돌을 처리하지 않는다(사망 후 크리티컬 등 효과음 방지).
	if (IsDead())
		return;

	ColliderTag tag = other->GetTag();

	if (tag == ColliderTag::ENEMY)
	{
		Enemy* enemy = static_cast<Enemy*>(other->GetOwner());
		if (!enemy || enemy->IsDying())   // 이미 죽는 중인 적은 재처리하지 않음.
			return;

		// 별 효과 중: 색/낙하 여부와 무관하게 적을 즉사시키고 플레이어는 무적.
		if (m_starTimer > 0)
		{
			enemy->Kill();
			return;
		}

		// 무적 중에는 적과 충돌하지 않는다.
		if (m_isInvincible)
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

	if (tag == ColliderTag::ITEM)
	{
		// 무적 중에도 획득 가능. 적은 이 블록을 안 타므로 플레이어 전용.
		Item* item = static_cast<Item*>(other->GetOwner());
		if (!item || item->IsDead())
			return;

		ApplyItem(item->GetType());
		item->Consume();
		SOUND->Play("item_pickup");
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

	// Kill()이 적 색을 빨강으로 바꾸므로, 크리티컬 판정에 쓸 원래 색을 먼저 저장한다.
	const Color enemyColor = enemy->GetColor();

	// 색과 무관하게 적은 접촉 즉시 사망시킨다.
	enemy->Kill();

	// Freeze(위치 고정) 중에는 실제로 멈춰 있으므로 충돌 시 속도 0(초록)으로 계산한다.
	// → 적과 색이 같을 수 없어 크리티컬이 나지 않고 피해를 입는다.
	const bool  frozen   = m_rigidbody->IsFrozen();
	const float hitSpeed = frozen ? 0.f : ColorSpeed();

	// 플레이어와 적의 색(=속도 단계)이 같으면 크리티컬: 반작용 0, 피해 없음.
	if (Enemy::ColorForSpeed(hitSpeed) == enemyColor)
	{
		m_rigidbody->SetVelocity(0.f);
		SOUND->Play("critical");
		// 사망/가시와 같은 방식의 차단형 진동으로 처치 순간을 확실히 흔든다(짧은 펀치감).
		ConsoleShakeRestore();
		ShakeConsoleWindow(CRIT_SHAKE_INTENSITY, CRIT_SHAKE_DURATION, CRIT_SHAKE_INTERVAL);
		return;
	}

	// 차징 중 피격이면 freeze를 풀어 넉백이 정상 적용되게 하고 누적을 리셋한다.
	if (frozen)
	{
		m_rigidbody->SetFrozen(false);
		m_chargedWheel = 0;
	}

	// 색이 다르면(크리티컬 X): 기존 로직(넉백) + 체력 1 감소.
	if (--m_hp <= 0)
	{
		// 체력이 0이 되면 패배. 전환 전에 HP 바를 0으로 갱신해 사망 진동 동안 0이 보이게 한다.
		// (좌표/형식은 GameScene::DrawHud의 HP 바와 동일해야 함)
		DrawBar(2, 0, "HP ", m_hp, m_maxHp, m_maxHp);
		ConsoleShakeRestore();
		ShakeConsoleWindow(DEATH_SHAKE_INTENSITY, DEATH_SHAKE_DURATION, DEATH_SHAKE_INTERVAL);
		SOUND->Play("player_death");
		SetDead();
		return;
	}

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

	m_chargedWheel = 0;   // 넉백 시 차징 누적 리셋(freeze 흔들림/미리보기 정리)

	// 넉백 후 무적 시작
	m_isInvincible    = true;
	m_invincibleTimer = PLAYER_INVINCIBLE_FRAMES;

	// 넉백 세기에 비례해 화면을 흔든다(방향 없는 진동).
	TriggerShake(knockMag * IMPACT_SHAKE_SCALE, IMPACT_SHAKE_FRAMES);
}

void Player::TriggerShake(float intensity, int frames)
{
	if (frames <= 0 || intensity <= 0.f)
		return;

	// 이미 더 강한 흔들림이 진행 중이면 덮어쓰지 않는다(이벤트 겹침 시 세기 약화 방지).
	if (m_shakeFrames > 0 && m_shakeIntensity >= intensity)
		return;

	m_shakeIntensity = intensity;
	m_shakeFrames    = frames;
	m_shakeFramesMax = frames;
}

void Player::UpdateInvincibility()
{
	if (!m_isInvincible)
		return;

	if (--m_invincibleTimer > 0)
		return;

	// 무적 해제
	m_isInvincible = false;

	// 해제 순간에도 적과 끼여 있으면: 랜덤 방향으로 튕기고 정상 피격과 동일하게 데미지도 준다.
	// (ApplyKnockback이 무적을 재부여하므로 끼인 채여도 피해는 60프레임당 1회로 제한된다.)
	if (!ColliderManager::GetInst()->IsOverlappingTag(m_collider.get(), ColliderTag::ENEMY))
		return;

	// 정상 피격(ResolveEnemyHit)과 동일한 체력 1 감소 + 사망 처리.
	if (--m_hp <= 0)
	{
		DrawBar(2, 0, "HP ", m_hp, m_maxHp, m_maxHp);
		ConsoleShakeRestore();
		ShakeConsoleWindow(DEATH_SHAKE_INTENSITY, DEATH_SHAKE_DURATION, DEATH_SHAKE_INTERVAL);
		SOUND->Play("player_death");
		SetDead();
		return;
	}

	int dir = (std::rand() % 2 == 0) ? 1 : -1;   // 랜덤 좌/우 (ASCII)
	ApplyKnockback(dir, KNOCKBACK_CONST);        // 다시 넉백 + 무적 재부여 (ASCII)
	SOUND->Play("hit");
}

void Player::UpdateCameraShake()
{
	// 이벤트 진동(넉백/적 처치)이 최우선. 세기를 남은 프레임 비율로 감쇠시키며 흔든다(방향 없음).
	if (m_shakeFrames > 0)
	{
		--m_shakeFrames;
		float t = static_cast<float>(m_shakeFrames) / m_shakeFramesMax;
		ConsoleShake(m_shakeIntensity * t);
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
		ConsoleShake(chargedSpeed * FREEZE_SHAKE_SCALE);
		return;
	}

	// 평상시: 플레이어 속도를 콘솔창 진동으로 표현
	float speed = std::fabs(m_rigidbody->GetVelocity());
	if (speed >= SHAKE_MIN_SPEED)
		ConsoleShake(speed * VELOCITY_SHAKE_SCALE);
	else
		ConsoleShakeRestore();
}

void Player::Render() const
{
	ClearSpeedText();   // 이전 프레임의 속도 수치 지우기(플레이어가 움직여도 잔상 안 남게)

	// 무적 중에는 깜빡인다.
	if (m_isInvincible)
	{
		bool hide = ((m_invincibleTimer / PLAYER_BLINK_INTERVAL) % 2) == 1;
		if (hide)
		{
			RemovePrevPos();
			m_hasLastRender = false;
			return;   // 깜빡임으로 숨는 프레임에는 속도 수치도 함께 숨긴다.
		}
	}

	Pawn::Render();
	DrawSpeedText();    // 플레이어 위에 현재 표시 속도 수치
}

void Player::DrawSpeedText() const
{
	int y = m_pos.y - 1;
	if (y < 0)
		return;   // 화면 위로 벗어나면 표시하지 않음

	float speed = ColorSpeed();
	char  buf[16];
	int   len = std::snprintf(buf, sizeof(buf), "%.1f", speed);
	if (len <= 0)
		return;

	int x = (m_pos.x < 0) ? 0 : m_pos.x;   // 플레이어 아이콘 바로 위

	GotoXY(x, y);
	SetColor(Enemy::ColorForSpeed(speed));   // 수치 색 = 속도 색(직관성)
	cout << buf;
	SetColor();

	m_lastSpeedPos = { x, y };
	m_lastSpeedLen = len;
	m_hasLastSpeed = true;
}

void Player::ClearSpeedText() const
{
	if (!m_hasLastSpeed)
		return;

	SetColor();
	GotoXY(m_lastSpeedPos.x, m_lastSpeedPos.y);
	for (int i = 0; i < m_lastSpeedLen; ++i)
		cout << ' ';

	m_hasLastSpeed = false;
}

void Player::ApplyItem(ItemType type)
{
	switch (type)
	{
	case ItemType::HEAL:
		if (m_hp < m_maxHp)
			++m_hp;   // HP 1 회복(최대치 초과 금지).
		break;

	case ItemType::STAR:
		m_starTimer = STAR_DURATION_FRAMES;
		m_renderIcon = "★";              // 플레이어가 별이 된다.
		m_lastPersistentPick = ItemType::STAR;
		break;

	case ItemType::GUN:
		m_gunTimer = GUN_DURATION_FRAMES;
		m_gunFireCounter = 1;            // 다음 프레임에 곧바로 첫 발.
		m_lastPersistentPick = ItemType::GUN;
		break;

	case ItemType::KILL_ALL:
		EnemyManager::GetInst()->KillAll();
		// 모두죽이기: CRIT 차단형 흔들림을 2배 인텐시티로.
		ConsoleShakeRestore();
		ShakeConsoleWindow(CRIT_SHAKE_INTENSITY * 2, CRIT_SHAKE_DURATION, CRIT_SHAKE_INTERVAL);
		break;

	default:
		break;
	}
}

void Player::UpdateItemEffects()
{
	// facing 갱신: 충분히 움직일 때만 방향 갱신, 정지 시 직전 값 유지.
	float vel = m_rigidbody->GetVelocity();
	if (vel > 0.1f)       m_facing = 1;
	else if (vel < -0.1f) m_facing = -1;

	// 별 타이머 감소(모습 복귀/종료 깜빡임은 UpdateAppearance가 담당).
	if (m_starTimer > 0)
		--m_starTimer;

	// 총 타이머 + 주기 발사.
	if (m_gunTimer > 0)
	{
		--m_gunTimer;
		if (--m_gunFireCounter <= 0)
		{
			BulletManager::GetInst()->Spawn(m_pos, m_facing);
			SOUND->Play("gun_fire");
			m_gunFireCounter = GUN_FIRE_INTERVAL;
		}
	}
}

ItemType Player::CurrentTintType()
{
	// 가장 최근에 먹은 지속 효과가 아직 살아있으면 그것, 아니면 남은 다른 효과.
	if (m_lastPersistentPick == ItemType::STAR && m_starTimer > 0) return ItemType::STAR;
	if (m_lastPersistentPick == ItemType::GUN  && m_gunTimer  > 0) return ItemType::GUN;
	if (m_starTimer > 0) { m_lastPersistentPick = ItemType::STAR; return ItemType::STAR; }
	m_lastPersistentPick = ItemType::GUN;
	return ItemType::GUN;   // 여기 도달 시 총은 반드시 활성.
}
