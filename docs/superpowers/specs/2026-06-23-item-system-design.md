# 아이템 시스템 설계

작성일: 2026-06-23

## 목표

하늘에서 떨어지는 아이템을 추가한다. 플레이어만 획득할 수 있고(무적 중에도 가능), 적은 획득할 수 없다. 아이템은 착지 후 일정 시간이 지나면 깜빡이다 사라진다. 4종: 회복, 별, 총, 모두죽이기.

기존 `Enemy` / `EnemyManager` 패턴을 그대로 미러링하여 일관성을 유지한다.

## 전체 구조

| 신규 요소 | 역할 |
|---|---|
| `ItemType` (enum) | `HEAL`, `STAR`, `GUN`, `KILL_ALL` |
| `Item : Pawn` | 중력 받아 떨어져 착지 → 깜빡이다 수명 끝 소멸. 타입/색/2아이콘 보유 |
| `ItemManager` (싱글턴) | 랜덤 간격 스폰, 수명/깜빡임 갱신, 동시 여러 개 관리 |
| `Bullet` (독립 클래스) | 중력 없이 수평 직진, 적/벽 충돌 처리 |
| `BulletManager` (싱글턴) | 총알 스폰/이동/소멸 관리 |
| `ColliderTag` 확장 | `ITEM`, `BULLET` 추가 |

## 컴포넌트 상세

### Item (`Item.h` / `Item.cpp`)

- `Pawn` 상속 → 적과 동일한 중력/착지 물리. 콜라이더 태그 `ITEM`. 콜라이더 폭/높이 1셀(Pawn 기본).
- 두 아이콘을 교대로 렌더해 깜빡임 표현:
  - `HEAL`: `♥` / `♡`
  - `STAR`: `★` / `☆`
  - `GUN`: `♠` / `♤`
  - `KILL_ALL`: `◑` / `◐`
- 아이템 색:
  - `HEAL = LIGHT_RED`
  - `STAR = LIGHT_YELLOW`
  - `GUN = CYAN`
  - `KILL_ALL = LIGHT_VIOLET`
- **수명/깜빡임은 프레임 기반**(총 수명 = `ITEM_LIFETIME_FRAMES`, 기본 8초 = 480프레임).
- 깜빡임 주기 = `FAST + (SLOW - FAST) * (남은 수명 비율)`.
  - 남은 수명이 많을수록 주기가 길어(느린 깜빡임), 끝에 가까울수록 연속적으로 짧아진다(점진 가속).
  - `SLOW`/`FAST`는 프레임 단위 상수(예: 30프레임 → 6프레임).
- 수명이 0이 되면 `SetDead()`.
- `Consume()`: 플레이어가 먹었을 때 호출, 내부적으로 `SetDead()` 표시. `ItemManager`가 다음 `Update`에서 제거.
- 충돌: `Item::OnCollision`은 `PLAYER`를 무시(획득은 Player가 전담), 그 외는 `Pawn::OnCollision`으로 위임해 `TILE` 착지만 처리. 적과는 서로 반응하지 않아 그냥 겹쳐 지나간다.

### ItemManager (`ItemManager.h` / `ItemManager.cpp`)

`EnemyManager`와 동일한 싱글턴 구조.

- `Init()` / `Update()` / `Render()` / `Clear()`.
- `TrySpawnItem(int groundLength)`: 랜덤 간격마다 호출되어 스폰.
  - 스폰 간격은 매번 `[ITEM_SPAWN_DELAY_MIN, ITEM_SPAWN_DELAY_MAX]`(기본 6~12초) 사이 랜덤.
  - 타입은 4종 균등 랜덤.
  - x 좌표는 지면 좌우 범위(`EnemyManager`의 스폰 x 계산과 동일) 내 랜덤, y = 0에서 낙하.
  - 동시 여러 개 허용(상한 없음 또는 넉넉한 상한).
- 스폰 타이밍/수명/깜빡임 전부 **프레임 기반 카운터**로 처리한다.
  - 설정창(오버레이) 중에는 `GameScene::Update`가 호출되지 않아 `ItemManager::Update`도 멈추므로 **자동으로 일시정지**된다. 따라서 `EnemyManager`와 달리 별도 `Pause()`/`Resume()` 배선이 필요 없다.

### Bullet (`Bullet.h` / `Bullet.cpp`)

`Rigidbody`에 중력 비활성 setter가 없어, `Pawn`을 쓰면 불필요한 중력이 붙는다. 따라서 **독립 경량 클래스**로 만든다.

- 멤버: `Vector2 m_pos`, `int m_dir`(±1), 자체 `Collider`(태그 `BULLET`).
- 매 프레임 `m_pos.x += m_dir * BULLET_SPEED`(적보다 빠르게).
- 충돌:
  - `ENEMY` → 적 `Kill()` + 자기 사망.
  - `TILE` → 자기 사망(벽 관통 X).
- 화면 밖으로 나가면 사망.
- 렌더는 이전 위치 지우기(clear-prev) 패턴. 아이콘/색은 단순한 표식(예: `-` 또는 작은 점).

### BulletManager (`BulletManager.h` / `BulletManager.cpp`)

싱글턴.

- `Spawn(Vector2 pos, int dir)`.
- `Update()`(이동 + 사망 제거) / `Render()` / `Clear()`.

## 획득 처리 (Player)

- `Player::OnCollision`에 `ITEM` 케이스 추가:
  - 소유 `Item`의 타입을 읽어 효과 적용 후 `item->Consume()`.
  - **무적 중에도 획득 가능**(무적 가드를 두지 않음).
  - `item_pickup` 사운드 재생.
- 적은 아이템에 반응하지 않는다(`Enemy`/`Item` 양쪽 콜백 모두 `ITEM`/`ENEMY`를 무시).

## 효과별 동작

모든 효과 타이머는 **Player의 프레임 카운터**라 오버레이 중 자동 정지된다.

### HEAL (즉시)
- `m_hp = min(m_maxHp, m_hp + 1)`. 틴트 없음.

### STAR (지속, 기본 5초 = 300프레임)
- 무적: 적과 충돌해도 데미지/넉백 없음.
- 적 접촉 즉사: 충돌한 적 `Kill()`.
- **낙하 중 적 즉사 판정(`!enemy->HasLanded()`)보다 별 체크가 먼저** 와서, 별 중에는 낙하 적에게도 안 죽고 오히려 적을 죽인다.
- 플레이어 아이콘 `●` → `★`(속도 색 대신 틴트색 사용).
- 종료 1초 전(60프레임)부터 깜빡임 → 종료 시 원래 모습 복귀.
- 처치 흔들림 없음(부드럽게 유지).

### GUN (지속, 기본 6초 = 360프레임)
- `m_facing`(마지막 이동 방향, 기본 우측 +1) 쪽으로 `GUN_FIRE_INTERVAL`(기본 0.6초 = 36프레임)마다 `BulletManager::Spawn` 호출.
- 발사 위치는 플레이어와 같은 줄(아이콘 위치)에서 시작.
- `m_facing`은 매 프레임 플레이어 수평 속도 부호로 갱신(정지 시 직전 값 유지).
- 발사 시 `gun_fire` 사운드.
- 처치 흔들림 없음.

### KILL_ALL (즉시)
- `EnemyManager::KillAll()`로 필드의 모든 적을 `Kill()`.
- **CRIT 차단형 흔들림(`ShakeConsoleWindow`)을 인텐시티 2배**로 호출(`CRIT_SHAKE_INTENSITY * 2`). 다른 차단형 흔들림과 동일하게 `ConsoleShakeRestore()` 후 호출.
- 틴트 없음.

### 지속 효과 틴트 (별/총)
- 별/총이 켜진 동안 플레이어 표시색 = **가장 최근에 먹은 지속 효과의 아이템 색**.
- 그 효과가 끝나면 남아 있는 다른 지속 효과 색으로, 둘 다 없으면 원래 속도 색으로 복귀.
- 별/총 동시 적용 가능(독립 타이머). 같은 효과 재획득 시 해당 타이머만 리셋.
- 크리티컬 판정은 `ColorSpeed()`(계산 속도)로 하므로, 표시색만 바뀌고 기계적 판정은 그대로 동작한다.

## 배선

- `GameScene::Update`: `ItemManager::TrySpawnItem(...)`, `ItemManager::Update()`, `BulletManager::Update()` 추가.
- `GameScene::Render`: `ItemManager::Render()`, `BulletManager::Render()` 추가.
- `GameScene::Release`: `ItemManager::Clear()`, `BulletManager::Clear()` 추가.
- `GameScene::Init`: `ItemManager::GetInst()->Init()` 추가.
- `Core::~Core`: `ItemManager`/`BulletManager` 싱글턴 `DestroyInst`.
- `Core` 생성자: `item_pickup`, `gun_fire` 사운드 `Load`(mp3는 Sounds 폴더에 나중에 채움).
- `EnemyManager`에 `KillAll()` 메서드 추가.
- 신규 상수는 전부 `Defines.h`에 모은다.

## 신규 상수 (Defines.h)

- `ITEM_SPAWN_DELAY_MIN`, `ITEM_SPAWN_DELAY_MAX` (초)
- `ITEM_LIFETIME_FRAMES` (기본 480)
- `ITEM_BLINK_SLOW_FRAMES`, `ITEM_BLINK_FAST_FRAMES`
- `STAR_DURATION_FRAMES` (기본 300), `STAR_BLINK_WARN_FRAMES` (기본 60)
- `GUN_DURATION_FRAMES` (기본 360), `GUN_FIRE_INTERVAL` (기본 36)
- `BULLET_SPEED`
- 아이템 색은 `ItemType` → `Color` 매핑 함수로 둔다.

## 사운드

- 신규: `item_pickup`(획득), `gun_fire`(발사).
- 재사용: `enemy_death`(별/총/킬올 처치는 `Enemy::Kill`이 이미 재생).

## 엣지 케이스

- 별 중 낙하 적 즉사 판정보다 별 보호가 우선.
- 무적 중에도 아이템 획득 가능.
- 모든 타이머가 프레임 기반 → 설정창 오버레이 중 자동 정지(별도 Pause/Resume 불필요).
- 오버레이 중에도 `ColliderManager::Update`는 돌지만, 매니저들이 갱신을 멈춰 새 아이템/총알이 생기거나 움직이지 않으므로 영향이 미미하다.
- 아이템 심볼(`♥★♠◑`)이 콘솔에서 1셀로 렌더될 수 있다. clear-prev가 2칸을 지우므로 시각적 문제는 없으나, 필요 시 폭을 조정한다.
