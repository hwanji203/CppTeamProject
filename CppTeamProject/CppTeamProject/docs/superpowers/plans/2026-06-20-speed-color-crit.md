# 속도-색 매칭 크리티컬 Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** 질량/power 개념을 제거하고, 플레이어 색(속도 state)과 적 색이 같으면 크리티컬 즉사, 다르면 넉백+HP 감소가 되도록 전투를 교체한다.

**Architecture:** 속도를 `SPEED_UNIT(0.5)` 단위 정수 state로 양자화하고, state→색 테이블(쉽게 변경)로 플레이어·적 색을 통일한다. 적은 색=속도(고정), 숫자=HP(별도 체력). 충돌은 state 비교로 크리티컬/넉백을 판정한다.

**Tech Stack:** C++ (콘솔 게임), Visual Studio (vcxproj). 테스트 프레임워크 없음 → 검증은 사용자가 직접 빌드/플레이.

## Global Constraints

- 빌드는 사용자가 직접 함. AI는 코드만 수정(빌드 명령 실행 금지). 각 태스크는 **항상 컴파일 가능한 상태**로 끝나야 함.
- 파일 인코딩 CP949: 한글 주석이 있는 파일을 Edit할 때 old_string은 ASCII 부분으로 매칭.
- 좌표계: 타일 1칸=화면 2칸, 플레이어/콜라이더는 화면 좌표. 본 작업은 좌표계 변경 없음.
- `SPEED_UNIT = 0.5f`, 적 속도 = `state × SPEED_UNIT`(state 1..SPEED_STATE_COUNT).
- 색 개수(state 개수)는 `Enemy.h`의 `SPEED_STATE_COLORS` 배열로만 정의 — 배열만 고치면 적 속도 단계·플레이어 최고 속도가 자동으로 따라감.
- 커밋은 사용자가 원할 때만. 아래 커밋 스텝은 실행 단계에서 사용자 승인 하에 수행.

---

### Task 1: 기반 추가 — 속도 state 인프라 (동작 변화 없음, 순수 추가)

**Files:**
- Modify: `Defines.h`
- Modify: `Enemy.h`
- Modify: `Enemy.cpp`

**Interfaces:**
- Produces:
  - `constexpr float SPEED_UNIT` (Defines.h)
  - `constexpr int ENEMY_HP_MAX` (Defines.h)
  - `inline constexpr Color SPEED_STATE_COLORS[]`, `inline constexpr int SPEED_STATE_COUNT` (Enemy.h)
  - `static int Enemy::SpeedStateOf(float speed)` — `round(speed/SPEED_UNIT)`, `[0, SPEED_STATE_COUNT]`로 클램프
  - `static Color Enemy::ColorForSpeedState(int state)` — state 1..N→색, 그 외→`Color::GRAY`
- 기존 `ColorForPower` / `GetForce` 등은 이 태스크에서 그대로 유지(빌드 보존).

- [ ] **Step 1: Defines.h에 상수 추가**

`KNOCKBACK_DURATION` 줄 위(또는 적 스폰 섹션)에 추가. 기존 `ENEMY_POWER_MAX`/`ENEMY_SPEED_*`/`ENEMY_FORCE_SCALE`/`CRIT_*`/`PLAYER_MASS`는 **아직 건드리지 않음**.

```cpp
// 속도 양자화: state = round(speed / SPEED_UNIT). 적 속도는 이 배수로만 부여됨.
constexpr float SPEED_UNIT   = 0.5f;   // ← 튜닝 노브
constexpr int   ENEMY_HP_MAX = 4;      // 적 HP 랜덤 상한(1~ENEMY_HP_MAX). 색=속도, HP=체력(별도)
```

- [ ] **Step 2: Enemy.h에 색 테이블 + state 함수 선언 추가**

클래스 선언 위(인클루드 다음, `class Enemy` 전)에 색 테이블 추가. `Color`는 `Pawn.h`(→`Console.h`)를 통해 사용 가능.

```cpp
// 속도 state 색. 이 배열에 색을 추가/삭제하면 state 개수(=적 속도 단계)와
// 플레이어 최고 속도가 자동으로 따라간다. ← 색을 쉽게 바꾸는 단일 지점.
inline constexpr Color SPEED_STATE_COLORS[] = {
    Color::LIGHT_RED,     // state 1 = 속도 0.5
    Color::LIGHT_YELLOW,  // state 2 = 속도 1.0
    Color::LIGHT_BLUE,    // state 3 = 속도 1.5
    Color::LIGHT_GREEN,   // state 4 = 속도 2.0
};
inline constexpr int SPEED_STATE_COUNT =
    static_cast<int>(sizeof(SPEED_STATE_COLORS) / sizeof(SPEED_STATE_COLORS[0]));
```

`public:` 섹션의 `static Color ColorForPower(int power);` 아래에 추가:

```cpp
    // 속도 → 정수 state(round, 0~SPEED_STATE_COUNT). 플레이어·적 공통.
    static int   SpeedStateOf(float speed);
    // state(1..N) → 색. 0 또는 범위 밖이면 중립색(GRAY).
    static Color ColorForSpeedState(int state);
```

- [ ] **Step 3: Enemy.cpp에 두 함수 구현 추가**

`ColorForPower` 정의 아래에 추가. `<cmath>`는 이미 포함됨, `SPEED_UNIT`은 `Defines.h`(포함됨)에서 옴.

```cpp
int Enemy::SpeedStateOf(float speed)
{
    int s = static_cast<int>(std::floor(speed / SPEED_UNIT + 0.5f));   // round
    if (s < 0)                 s = 0;
    if (s > SPEED_STATE_COUNT) s = SPEED_STATE_COUNT;
    return s;
}

Color Enemy::ColorForSpeedState(int state)
{
    if (state < 1 || state > SPEED_STATE_COUNT)
        return Color::GRAY;   // state 0(정지) 등 = 중립색
    return SPEED_STATE_COLORS[state - 1];
}
```

- [ ] **Step 4: 빌드 확인 (사용자)**

기대: 추가만 했으므로 정상 컴파일, 게임 동작은 기존과 동일(아직 아무것도 사용 안 함).

- [ ] **Step 5: Commit (사용자 승인 시)**

```bash
git add Defines.h Enemy.h Enemy.cpp
git commit -m "feat: add speed-state color infrastructure (SPEED_UNIT, color table, state helpers)"
```

---

### Task 2: 적을 hp+state 모델로 전환 + 충돌 판정 교체

**Files:**
- Modify: `Enemy.h`
- Modify: `Enemy.cpp`
- Modify: `EnemyManager.cpp`
- Modify: `Player.cpp:119-158` (`ResolveEnemyHit`)

**Interfaces:**
- Consumes: Task 1의 `SpeedStateOf`, `ColorForSpeedState`, `SPEED_STATE_COUNT`, `SPEED_UNIT`, `ENEMY_HP_MAX`.
- Produces:
  - `Enemy(const Vector2* playerPos, int hp, int state, Vector2 pos)` 생성자
  - `int Enemy::GetHp() const`
  - `int Enemy::GetSpeedState() const`
  - `Enemy::GetForce()`, `GetPower()`, `GetSpeed()`, `m_power` 제거
- `ColorForPower`는 **이 태스크에서 유지**(HUD가 아직 사용). Task 3에서 제거.

- [ ] **Step 1: Enemy.h — 생성자/게터/멤버 교체**

생성자 시그니처 변경:
```cpp
	// hp = 체력(정수), state = 속도 단계(1..SPEED_STATE_COUNT). 색=state, 속도=state*SPEED_UNIT.
	Enemy(const Vector2* playerPos, int hp, int state, Vector2 pos = { 0, 0 });
```

게터 교체 — 기존 `GetPower`/`GetSpeed`/`GetForce` 3줄을 다음으로:
```cpp
	int   GetHp()         const { return m_hp; }
	int   GetSpeedState() const { return m_state; }   // 색 매칭용 속도 state
```

멤버 교체 — 기존 `int m_power;` / `float m_speed;` 2줄을:
```cpp
	int   m_hp;      // 체력(충돌 1회당 1 감소, 0이면 사망)
	int   m_state;   // 속도 단계(색 결정, 고정)
	float m_speed;   // 추적 속도 = m_state * SPEED_UNIT
```

- [ ] **Step 2: Enemy.cpp — 생성자/TakeHit/Render 교체, GetForce·GetSpeed 제거**

생성자:
```cpp
Enemy::Enemy(const Vector2* playerPos, int hp, int state, Vector2 pos)
	: Pawn(pos, ColorForSpeedState(state), "■", { SCREEN_WIDTH, SCREEN_HEIGHT }, ColliderTag::ENEMY),
	m_hp(hp),
	m_state(state),
	m_speed(state * SPEED_UNIT),
	m_playerPos(playerPos)
{
	m_rigidbody->SetMaxSpeed(m_speed);   // 속도 = state * SPEED_UNIT
}
```

`GetSpeed()`와 `GetForce()` 정의 **삭제**.

`TakeHit()` 교체(색 갱신 제거 — 색은 속도 state 고정):
```cpp
void Enemy::TakeHit()
{
	if (m_isDying)
		return;

	if (--m_hp <= 0)
		EnterDeath();
	// 색은 속도 state로 고정이므로 HP가 변해도 갱신하지 않는다.
}
```

`Render()` 교체(힘 → HP 숫자, 흰색):
```cpp
void Enemy::Render() const
{
	Pawn::Render();

	// 머리 위에 HP 표시(색=속도 state는 글리프가 나타냄).
	char buf[16];
	std::snprintf(buf, sizeof(buf), "%d", m_hp);
	SetColor(Color::WHITE);
	GotoXY(m_pos.x, m_pos.y - 1);
	cout << buf;
}
```

- [ ] **Step 3: EnemyManager.cpp — 스폰 파라미터 교체**

`TrySpawnEnemyInRandomPos`의 `power`/`speed` 계산(기존 63~67줄)을 교체:
```cpp
        int hp    = 1 + std::rand() % ENEMY_HP_MAX;          // 1 ~ ENEMY_HP_MAX
        int state = 1 + std::rand() % SPEED_STATE_COUNT;     // 1 ~ SPEED_STATE_COUNT

        auto enemy = std::make_unique<Enemy>(playerPos, hp, state, Vector2{ spawnX, 0 });
        m_enemys.push_back(move(enemy));
```
(EnemyManager.cpp는 EnemyManager.h를 통해 Enemy.h를 포함하므로 `SPEED_STATE_COUNT` 사용 가능. 확인만.)

- [ ] **Step 4: Player.cpp — ResolveEnemyHit를 state 비교로 교체**

`Player::ResolveEnemyHit` 전체(119~158줄)를 교체:
```cpp
void Player::ResolveEnemyHit(Enemy* enemy, Collider* enemyCol)
{
	const float playerVel   = m_rigidbody->GetVelocity();
	const int   playerState = Enemy::SpeedStateOf(std::fabs(playerVel));
	const int   enemyState  = enemy->GetSpeedState();

	// 색(속도 state)이 같으면 크리티컬 즉사(반작용 0, 흔들림 없음).
	if (playerState == enemyState)
	{
		m_rigidbody->SetVelocity(0.f);
		enemy->Kill();
		SOUND->Play("critical");
		return;
	}

	// 진행(속도) 방향. 속도가 0이면 적을 향하는 방향을 진행 방향으로 본다.
	int travelDir = (playerVel > 0.f) ? 1 : (playerVel < 0.f) ? -1 : 0;
	if (travelDir == 0)
	{
		int enemyCenterX = (enemyCol->GetLeft() + enemyCol->GetRight()) / 2;
		travelDir = (m_pos.x < enemyCenterX) ? 1 : -1;
	}

	// 내가 더 빠르면(state 큼) 진행 방향으로 밀고, 느리면 튕겨 나온다. 세기는 일정.
	int knockDir = (playerState > enemyState) ? travelDir : -travelDir;

	ApplyKnockback(knockDir, KNOCKBACK_CONST);
	enemy->TakeHit();              // 일반 충돌: HP 1 감소(0이면 사망)
	SOUND->Play("hit");
}
```

- [ ] **Step 5: 빌드 확인 (사용자)**

기대: 정상 컴파일. 게임에서 — 적 색이 속도(빠를수록 초록 쪽)로 표시되고 머리 위에 HP 숫자. 플레이어 속도 state가 적 색과 같을 때 부딪히면 즉사(critical 사운드), 다르면 넉백+HP 감소. HUD 범례는 아직 옛 power 범례(다음 태스크에서 교체).

- [ ] **Step 6: Commit (사용자 승인 시)**

```bash
git add Enemy.h Enemy.cpp EnemyManager.cpp Player.cpp
git commit -m "feat: enemy uses hp+speed-state; crit by player/enemy state match"
```

---

### Task 3: 플레이어 색/표시 전환 + HUD 범례 + 죽은 상수 정리

**Files:**
- Modify: `Player.cpp` (생성자, `Tick`, `Render`, `RemovePrevPos`)
- Modify: `GameScene.cpp:72-98` (`DrawHud`)
- Modify: `GameScene.h:21` (주석)
- Modify: `Defines.h` (미사용 상수 제거)
- Modify: `Enemy.h` / `Enemy.cpp` (`ColorForPower` 제거)

**Interfaces:**
- Consumes: `Enemy::SpeedStateOf`, `Enemy::ColorForSpeedState`, `SPEED_STATE_COUNT`, `SPEED_UNIT`.
- 제거: `ColorForPower`, `ENEMY_POWER_MAX`, `ENEMY_SPEED_MIN/MAX`, `ENEMY_FORCE_SCALE`, `CRIT_TOLERANCE`, `CRIT_MIN_BAND`, `PLAYER_MASS`, `KNOCKBACK_FORCE_SCALE`.

- [ ] **Step 1: Player.cpp — 생성자에서 maxSpeed 설정**

```cpp
Player::Player(Vector2 pos)
	: Pawn(pos, Color::CYAN, "●", { SCREEN_WIDTH, SCREEN_HEIGHT }, ColliderTag::PLAYER),
	m_canMove(true)
{
	// 색을 늘리면(SPEED_STATE_COUNT 증가) 최고 속도도 자동으로 올라가 모든 state 도달 가능.
	m_rigidbody->SetMaxSpeed(SPEED_STATE_COUNT * SPEED_UNIT);
}
```

- [ ] **Step 2: Player.cpp — Tick 시작에 글리프 색 갱신 추가**

`void Player::Tick()` 본문 첫 줄에 추가(모든 분기/early-return 전이라 매 프레임 반영):
```cpp
	// 글리프 색 = 현재 속도 state 색(적 색과 같아지는 순간이 크리티컬 타이밍).
	m_renderColor = Enemy::ColorForSpeedState(Enemy::SpeedStateOf(std::fabs(m_rigidbody->GetVelocity())));
```

- [ ] **Step 3: Player.cpp — Render에서 힘 숫자 제거**

`Player::Render()`의 `Pawn::Render();` 아래 힘 숫자 출력 블록(buf/force/snprintf/SetColor/GotoXY/cout) **삭제**. 깜빡임 처리와 `Pawn::Render();`는 유지. 결과:
```cpp
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
```

- [ ] **Step 4: Player.cpp — RemovePrevPos에서 숫자 잔상 제거 코드 삭제**

머리 위 숫자가 없어졌으므로 잔상 지우기 불필요. 본문을 베이스 호출만 남김:
```cpp
void Player::RemovePrevPos() const
{
	Pawn::RemovePrevPos();
}
```

- [ ] **Step 5: GameScene.cpp — HUD 범례를 속도-state로 교체**

`DrawHud`의 범례 루프(85~97줄, `ENEMY_POWER_MAX`/`ColorForPower` 사용부)를 교체:
```cpp
	// 범례: 속도-state 색(위에서부터 N..1). 색=속도, 숫자=state.
	const int legendX = SCREEN_WIDTH - 22;
	const int topY    = hudY - (SPEED_STATE_COUNT / 2);
	for (int s = SPEED_STATE_COUNT; s >= 1; --s)
	{
		GotoXY(legendX, topY + (SPEED_STATE_COUNT - s));
		SetColor(Color::WHITE);
		cout << "(";
		SetColor(Enemy::ColorForSpeedState(s));
		cout << "■";
		SetColor(Color::WHITE);
		cout << ": " << s << ")";
	}
```

- [ ] **Step 6: GameScene.h — 주석 갱신**

21줄 `// 우측 HUD: 버틴 초 + 적 power 색 범례` → `// 우측 HUD: 버틴 초 + 속도 state 색 범례`.

- [ ] **Step 7: Enemy.h / Enemy.cpp — ColorForPower 제거**

Enemy.h의 `static Color ColorForPower(int power);` 선언과 위 주석 삭제. Enemy.cpp의 `Color Enemy::ColorForPower(int power){...}` 정의 삭제. (이제 사용처 없음 — HUD는 Step 5에서 전환됨.)

- [ ] **Step 8: Defines.h — 미사용 상수 제거**

다음을 삭제: `ENEMY_POWER_MAX`, `ENEMY_SPEED_MIN`, `ENEMY_SPEED_MAX`, `ENEMY_FORCE_SCALE`, `PLAYER_MASS`, `CRIT_TOLERANCE`, `CRIT_MIN_BAND`, `KNOCKBACK_FORCE_SCALE`. 관련 설명 주석도 정리. `KNOCKBACK_MAX`, `KNOCKBACK_CONST`, `SPEED_UNIT`, `ENEMY_HP_MAX` 등은 유지.

- [ ] **Step 9: 빌드 확인 (사용자)**

기대: 정상 컴파일. 플레이어 `●` 색이 속도에 따라 변하고 머리 위 숫자 없음. 적은 색=속도/숫자=HP. HUD 범례가 속도 state 색으로 표시. 같은 색끼리 부딪히면 크리티컬, 다르면 넉백+HP 감소.

- [ ] **Step 10: Commit (사용자 승인 시)**

```bash
git add Player.cpp GameScene.cpp GameScene.h Enemy.h Enemy.cpp Defines.h
git commit -m "feat: player color by speed-state, HP-only labels, speed-state HUD legend; drop dead force/power constants"
```

---

## Self-Review 결과

- **스펙 커버리지**: SPEED_UNIT/색테이블(T1), state함수(T1), 적 hp+state·색고정·HP표시(T2), 충돌 state비교·넉백방향(T2), 플레이어 색/숫자제거/maxSpeed(T3), HUD범례(T3), 죽은 상수/ColorForPower 정리(T3) — 스펙 전 항목 매핑됨.
- **YAGNI 비목표 준수**: `SetCritReady`/`Player::GetSpeedState` 외부 노출/적 숫자 빨강 신호 — 만들지 않음(색 매칭이 신호).
- **타입 일관성**: `SpeedStateOf(float)→int`, `ColorForSpeedState(int)→Color`, `GetSpeedState()→int`, `Enemy(playerPos, hp, state, pos)` — 모든 태스크에서 동일하게 사용.
- **빌드 보존**: 각 태스크 종료 시점이 컴파일 가능(추가→전환→정리 순서로 사용처와 정의를 같은 태스크에 묶음).
- **선결 수정**: `KNOCKBACK_MAX` 누락은 이번 세션에서 이미 Defines.h에 추가함(기존 브랜치 빌드 에러 해소).
