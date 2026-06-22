# 속도-색 매칭 크리티컬 설계 (질량 제거)

날짜: 2026-06-20
대상: CppTeamProject (콘솔 플랫포머)

## 목표 / 의도

기존 크리티컬은 `|playerForce - enemyForce| <= critBand`(연속 구간)라 "얼마나 가까워야 크리티컬인지" 플레이어가 알 수 없었다.
이를 **속도를 정수 state로 양자화하고, 플레이어 색과 적 색이 같으면 크리티컬**이 되는 직관적 방식으로 교체한다.

- **질량(mass)/power 개념 제거**: 매칭은 오직 "속도"로. 플레이어는 숫자를 보지 않고 자기 `●` 색과 적 `■` 색을 맞추면 된다.
- 힘 숫자가 사라진 적 머리 위 자리에는 **HP(체력 전용 정수)**를 표시한다.

## 핵심 데이터

### SPEED_UNIT (Defines.h)
```cpp
constexpr float SPEED_UNIT = 0.5f;   // 속도 양자화 단위(튜닝 노브). 적 속도는 이 배수로만 부여됨.
```

### 색 테이블 (Enemy.h) — 쉽게 변경 가능
```cpp
// 이 배열에 색을 추가/삭제하면 state 개수(=적 속도 단계)와 플레이어 최고 속도가 자동으로 따라감.
inline constexpr Color SPEED_STATE_COLORS[] = {
    Color::LIGHT_RED,     // state 1 = 속도 0.5
    Color::LIGHT_YELLOW,  // state 2 = 속도 1.0
    Color::LIGHT_BLUE,    // state 3 = 속도 1.5
    Color::LIGHT_GREEN,   // state 4 = 속도 2.0
};
inline constexpr int SPEED_STATE_COUNT =
    static_cast<int>(sizeof(SPEED_STATE_COLORS) / sizeof(SPEED_STATE_COLORS[0]));
```
- `state n` ↔ 속도 `n × SPEED_UNIT`, 색 `SPEED_STATE_COLORS[n-1]`.
- Color enum은 Console.h에 있고 Enemy.h는 Pawn.h(→Console.h)를 포함하므로 헤더에 둘 수 있다.

## 속도 → state 함수 (플레이어·적 공통, Enemy의 static)

```cpp
static int   SpeedStateOf(float speed);      // round(speed / SPEED_UNIT), [0, SPEED_STATE_COUNT]로 클램프
static Color ColorForSpeedState(int state);  // state 1..N → 색, state 0 → 중립(예: GRAY/WHITE)
```
- `SpeedStateOf` = `round(speed / SPEED_UNIT)`. state 0 = 정지(매칭 불가 → 크리티컬 남발 방지).
- 적은 항상 state 1~N이므로 플레이어가 가만히 있으면(state 0) 절대 크리티컬이 나지 않는다.
- 매칭 윈도우: 각 state n은 속도 `[(n-0.5)·unit, (n+0.5)·unit)` 폭(=0.5단위)을 가진다.

## 적 (Enemy)

- 생성 시 **랜덤 state 1~SPEED_STATE_COUNT** 선택 → `m_speed = state × SPEED_UNIT`.
- **글리프 `■` 색 = 그 state의 색(고정, HP 변해도 색 안 바뀜)**.
- **HP**: 1~`ENEMY_HP_MAX`(=4) 랜덤, 별도 정수. 머리 위에 **HP 숫자(흰색)** 표시(기존 힘 숫자 자리).
- 생성자 시그니처: 기존 `Enemy(playerPos, power, speed, pos)` → `Enemy(playerPos, hp, state, pos)`
  (state로 speed/color를 내부에서 도출). speed를 직접 받지 않고 state만 받아 일관성 보장.
- 제거: `GetForce()`, `m_power`, `GetPower()`, 힘 표시 로직.
- `TakeHit()`은 HP를 1 감소(0이면 사망)하되 **색은 갱신하지 않음**(색은 속도 state 고정).
- `Kill()`(크리티컬 즉사)은 유지.

## 플레이어 (Player)

- **글리프 `●` 색 = 자기 현재 속도 state 색**. 매 프레임 `m_renderColor = ColorForSpeedState(SpeedStateOf(|velocity|))`로 갱신(Render 또는 Tick에서 Pawn::Render 직전).
- **머리 위 숫자 제거**. `Player::Render`의 힘 숫자 출력과 `Player::RemovePrevPos`의 숫자 잔상 제거 코드를 삭제.
- 생성자에서 `m_rigidbody->SetMaxSpeed(SPEED_STATE_COUNT × SPEED_UNIT)` 설정 → 색을 늘리면 최고 속도도 자동으로 늘어 모든 state 도달 가능.
- 주의: 마찰(friction 0.85)로 속도가 매 프레임 감속하므로 플레이어 색은 계속 바뀐다. "원하는 색일 때 적에게 부딪히기"는 휠 속도 조절 + 타이밍 스킬이 된다(의도된 난이도, 보정 없음).

## 충돌 처리 (`Player::ResolveEnemyHit`)

```text
playerState = SpeedStateOf(|playerVel|)
enemyState  = enemy->GetSpeedState()

if (playerState == enemyState):
    크리티컬 → SetVelocity(0); enemy->Kill(); SOUND->Play("critical")  // 반작용 0, 흔들림 없음
else:
    travelDir = 진행 방향(속도 부호, 0이면 적 쪽)
    knockDir  = (playerState > enemyState) ? travelDir : -travelDir   // 내가 더 빠르면 밀고, 느리면 튕김
    ApplyKnockback(knockDir, KNOCKBACK_CONST)
    enemy->TakeHit()          // HP 1 감소(0이면 사망)
    SOUND->Play("hit")
```
- 넉백 세기는 항상 `KNOCKBACK_CONST`(기존 유지). 방향만 state 비교로 결정.

## HUD 범례 (`GameScene::DrawHud`)

- 기존 power 색 범례 → **속도-state 범례**로 교체.
- state 1~`SPEED_STATE_COUNT`를 세로로, 각 state의 색 `■`와 속도값(또는 state 번호) 표시.
- `ENEMY_POWER_MAX` 의존부를 `SPEED_STATE_COUNT`로 교체.

## 정리(제거할 것)

- Defines.h: `CRIT_TOLERANCE`, `CRIT_MIN_BAND`, `PLAYER_MASS`, `ENEMY_FORCE_SCALE`, `ENEMY_SPEED_MIN/MAX`, `KNOCKBACK_FORCE_SCALE`(이미 미사용). `ENEMY_POWER_MAX` → `ENEMY_HP_MAX`로 의미 변경(또는 신규).
- Enemy: `GetForce`, `m_power`/`GetPower`, `ColorForPower`(→ `ColorForSpeedState`로 대체).
- Player: 힘 숫자 표시/잔상 제거 코드.
- EnemyManager 스폰: `power`/`speed` 랜덤 → `hp`(1~ENEMY_HP_MAX) + `state`(1~SPEED_STATE_COUNT) 랜덤으로 교체.

## 명시적 비목표 (YAGNI)

- "크리 가능"을 알리는 별도 신호(적 HP 숫자 빨강, `SetCritReady`, `Player::GetSpeedState` 외부 노출)는 **만들지 않는다**. 색 매칭 자체가 신호다.
- 마찰 타이밍 보정 없음.

## 영향 파일

- `Defines.h` — 상수 추가/제거.
- `Enemy.h` / `Enemy.cpp` — 색 테이블, state 함수, 생성자, HP, Render.
- `Player.h` / `Player.cpp` — 색 갱신, 숫자 제거, maxSpeed, ResolveEnemyHit.
- `EnemyManager.cpp` — 스폰 파라미터.
- `GameScene.cpp` / `GameScene.h` — HUD 범례.

## 좌표계 / 빌드 주의

- 좌표계: 타일 1칸=화면 2칸, 플레이어/콜라이더는 화면 좌표(변경 없음).
- 빌드는 사용자가 직접 함(코드만 수정).
- 파일 인코딩 CP949: 한글 주석 Edit 시 old_string은 ASCII 부분으로 매칭.
