# 아이템 시스템 구현 계획

> **For agentic workers:** 이 프로젝트는 자동 테스트 프레임워크가 없고(콘솔 게임), **빌드와 커밋은 사용자가 직접** 한다([[feedback_no_build]], [[feedback_user_commits]]). 따라서 각 Task는 "수정할 파일 + 완성 코드 + 빌드 후 눈으로 확인할 동작"으로 구성한다. Claude는 `git commit`/빌드를 실행하지 않는다.

**Goal:** 하늘에서 떨어지는 4종 아이템(회복/별/총/모두죽이기)을 추가한다. 플레이어만 획득 가능(무적 중에도), 적은 못 먹는다.

**Architecture:** 기존 `Enemy`/`EnemyManager` 싱글턴 패턴을 미러링한다. `Item:Pawn`(중력 낙하)을 `ItemManager` 싱글턴이 관리하고, 총은 중력 없는 독립 `Bullet`을 `BulletManager` 싱글턴이 관리한다. 효과 상태는 전부 Player의 프레임 카운터로 들고 있어 설정창 오버레이 중 자동 정지된다.

**Tech Stack:** C++ (Visual Studio, Windows 콘솔), 기존 `Pawn`/`Rigidbody`/`Collider`/`ColliderManager`/`SoundManager` 재사용.

## Global Constraints

- **인코딩:** 신규 .h/.cpp 파일은 기존 프로젝트 파일과 동일한 인코딩(CP949/ANSI)으로 저장해야 한다. 아이콘 글리프(`♥♡★☆♠♤◑◐`)와 한글 주석이 깨지면 컴파일/렌더가 실패한다.
- **CP949 주석 함정([[project_cp949_comment_pitfall]]):** 코드 옆 한글 `//` 주석은 반드시 ASCII 문자(예: `.`)로 끝낸다. 안 그러면 다음 선언이 주석에 먹혀 컴파일 에러가 난다.
- **좌표계([[project_coordinate_system]]):** `Vector2`는 `int`. 위치는 화면 좌표.
- **싱글턴 파괴 순서:** 콜라이더를 들고 있는 매니저(Item/Bullet)는 `ColliderManager::DestroyInst`보다 먼저 파괴/Clear 한다.
- 빌드/커밋은 사용자가 한다. Claude는 코드만 작성한다.

---

## 파일 구조

신규:
- `Item.h` / `Item.cpp` — `ItemType` enum + `Item:Pawn`(낙하/수명/깜빡임/타입별 색·아이콘).
- `ItemManager.h` / `ItemManager.cpp` — 싱글턴, 랜덤 스폰/갱신/렌더.
- `Bullet.h` / `Bullet.cpp` — 중력 없는 수평 총알.
- `BulletManager.h` / `BulletManager.cpp` — 싱글턴, 총알 스폰/갱신/렌더.

수정:
- `Collider.h` — `ColliderTag`에 `ITEM`, `BULLET` 추가.
- `Defines.h` — 신규 상수.
- `EnemyManager.h` / `EnemyManager.cpp` — `KillAll()` 추가.
- `Player.h` / `Player.cpp` — 효과 상태/획득/발사/별 렌더/틴트.
- `GameScene.cpp` — Item/Bullet 매니저 배선.
- `Core.cpp` — 싱글턴 파괴 + 사운드 로드.

---

## Task 1: ColliderTag 확장 + Defines 상수

**Files:**
- Modify: `Collider.h:5`
- Modify: `Defines.h` (끝에 추가)

**Interfaces:**
- Produces: `ColliderTag::ITEM`, `ColliderTag::BULLET`. 신규 상수들(아래 값 그대로).

- [ ] **Step 1:** `Collider.h` 5번 줄의 enum을 수정한다.

```cpp
enum class ColliderTag { TILE, PLAYER, ENEMY, SPIKE, ITEM, BULLET };
```

- [ ] **Step 2:** `Defines.h` 맨 끝에 아래 상수 블록을 추가한다.

```cpp

// ── 아이템 시스템 ─────────────────────────────────────────────
// 스폰 간격은 매번 [MIN, MAX] 초 사이 랜덤. 프레임 기반이라 오버레이 중 자동 정지.
constexpr float ITEM_SPAWN_DELAY_MIN = 6.0f;   // 아이템 최소 스폰 간격(초).
constexpr float ITEM_SPAWN_DELAY_MAX = 12.0f;  // 아이템 최대 스폰 간격(초).

constexpr int ITEM_LIFETIME_FRAMES   = 480;    // 아이템 총 수명(프레임, 60fps=8초).
constexpr int ITEM_BLINK_SLOW_FRAMES = 30;     // 수명 많을 때 깜빡임 주기(느림).
constexpr int ITEM_BLINK_FAST_FRAMES = 6;      // 수명 적을 때 깜빡임 주기(빠름).

constexpr int STAR_DURATION_FRAMES   = 300;    // 별 효과 지속(프레임, 5초).
constexpr int STAR_BLINK_WARN_FRAMES = 60;     // 별 종료 전 깜빡임 시작(프레임, 1초).

constexpr int GUN_DURATION_FRAMES    = 360;    // 총 효과 지속(프레임, 6초).
constexpr int GUN_FIRE_INTERVAL      = 36;     // 총알 발사 간격(프레임, 0.6초).

constexpr float BULLET_SPEED = 3.0f;           // 총알 수평 속도(셀/프레임, 플레이어보다 빠름).
```

- [ ] **Step 3 (검증):** 사용자가 빌드. 컴파일만 통과하면 OK(아직 동작 변화 없음).

---

## Task 2: Item 클래스

**Files:**
- Create: `Item.h`
- Create: `Item.cpp`

**Interfaces:**
- Consumes: `ColliderTag::ITEM`(Task 1), `Pawn`, `Color`, `Defines.h` 상수.
- Produces:
  - `enum class ItemType { HEAL, STAR, GUN, KILL_ALL, COUNT };`
  - `Item(ItemType type, Vector2 pos);`
  - `ItemType Item::GetType() const;`
  - `Color Item::GetColor() const;`
  - `void Item::Consume();`  // 먹었을 때 호출, 내부 SetDead.
  - `static Color Item::ColorForType(ItemType t);`

- [ ] **Step 1:** `Item.h` 생성.

```cpp
#pragma once
#include "Pawn.h"
#include <string>

enum class ItemType { HEAL, STAR, GUN, KILL_ALL, COUNT };

// 하늘에서 떨어져 착지 후 깜빡이다 사라지는 아이템. 플레이어만 획득.
class Item : public Pawn
{
public:
	Item(ItemType type, Vector2 pos = { 0, 0 });

	virtual void Tick() override;

	ItemType GetType()  const { return m_type; }
	Color    GetColor() const { return m_renderColor; }   // 타입별 고정 색.

	void Consume() { SetDead(); }   // 플레이어가 먹으면 사망 표시, 매니저가 다음 Update에서 제거.

	// 타입 -> 고정 색. 지속형(별/총) 틴트에도 같은 매핑을 쓴다.
	static Color ColorForType(ItemType t);

private:
	virtual void OnCollision(Collider* other) override;

	// 남은 수명 비율로 깜빡임 주기를 보간(많이 남으면 느리게, 끝나갈수록 빠르게).
	int CurrentBlinkPeriod() const;

	ItemType    m_type;
	int         m_lifeFrames;          // 남은 수명(프레임).
	int         m_blinkCounter = 0;    // 현재 주기 경과 프레임.
	bool        m_showBright   = true; // true면 밝은 아이콘, false면 어두운 아이콘.
	std::string m_brightIcon;
	std::string m_darkIcon;
};
```

- [ ] **Step 2:** `Item.cpp` 생성.

```cpp
#include "Item.h"
#include "Defines.h"

// 타입별 두 아이콘(밝은/어두운)과 색.
static const char* BrightIcon(ItemType t)
{
	switch (t)
	{
	case ItemType::HEAL:     return "♥";
	case ItemType::STAR:     return "★";
	case ItemType::GUN:      return "♠";
	case ItemType::KILL_ALL: return "◑";
	default:                 return "?";
	}
}

static const char* DarkIcon(ItemType t)
{
	switch (t)
	{
	case ItemType::HEAL:     return "♡";
	case ItemType::STAR:     return "☆";
	case ItemType::GUN:      return "♤";
	case ItemType::KILL_ALL: return "◐";
	default:                 return "?";
	}
}

Color Item::ColorForType(ItemType t)
{
	switch (t)
	{
	case ItemType::HEAL:     return Color::LIGHT_RED;
	case ItemType::STAR:     return Color::LIGHT_YELLOW;
	case ItemType::GUN:      return Color::CYAN;
	case ItemType::KILL_ALL: return Color::LIGHT_VIOLET;
	default:                 return Color::WHITE;
	}
}

Item::Item(ItemType type, Vector2 pos)
	: Pawn(pos, ColorForType(type), BrightIcon(type),
		   { SCREEN_WIDTH, SCREEN_HEIGHT }, ColliderTag::ITEM)
	, m_type(type)
	, m_lifeFrames(ITEM_LIFETIME_FRAMES)
	, m_brightIcon(BrightIcon(type))
	, m_darkIcon(DarkIcon(type))
{
}

int Item::CurrentBlinkPeriod() const
{
	// 남은 수명 비율 r(0~1). r이 크면 느린 주기, 작으면 빠른 주기로 선형 보간.
	float r = (float)m_lifeFrames / (float)ITEM_LIFETIME_FRAMES;
	if (r < 0.f) r = 0.f;
	if (r > 1.f) r = 1.f;
	float period = ITEM_BLINK_FAST_FRAMES
		+ (ITEM_BLINK_SLOW_FRAMES - ITEM_BLINK_FAST_FRAMES) * r;
	int p = (int)period;
	return (p < 1) ? 1 : p;   // 최소 1프레임.
}

void Item::OnCollision(Collider* other)
{
	// 획득은 Player가 전담하므로 플레이어와의 충돌은 무시한다.
	if (other->GetTag() == ColliderTag::PLAYER)
		return;

	Pawn::OnCollision(other);   // TILE 착지만 처리(적과는 서로 반응 안 함).
}

void Item::Tick()
{
	Pawn::Tick();   // 중력/착지.

	if (--m_lifeFrames <= 0)
	{
		SetDead();
		return;
	}

	// 깜빡임: 현재 주기마다 밝은/어두운 아이콘 교대(색은 고정).
	if (++m_blinkCounter >= CurrentBlinkPeriod())
	{
		m_blinkCounter = 0;
		m_showBright = !m_showBright;
		m_renderIcon = m_showBright ? m_brightIcon : m_darkIcon;
	}
}
```

- [ ] **Step 3:** Visual Studio 프로젝트(.vcxproj)에 `Item.h`/`Item.cpp`를 포함시킨다(솔루션 탐색기에서 기존 항목 추가, 또는 .vcxproj/.filters 편집).

- [ ] **Step 4 (검증):** 사용자가 빌드. 컴파일 통과 확인. (아직 스폰 안 됨 → 다음 Task에서 확인.)

---

## Task 3: ItemManager 싱글턴

**Files:**
- Create: `ItemManager.h`
- Create: `ItemManager.cpp`

**Interfaces:**
- Consumes: `Item`(Task 2), `Defines.h` 상수, `Utils.h`(SAFE_DELETE).
- Produces:
  - `static ItemManager* ItemManager::GetInst();`
  - `static void ItemManager::DestroyInst();`
  - `void Init(); void Update(); void Render(); void Clear();`
  - `void TrySpawnItem(int groundLength);`

- [ ] **Step 1:** `ItemManager.h` 생성. (`EnemyManager.h`와 동일 구조.)

```cpp
#pragma once
#include "Utils.h"
#include <vector>
#include <memory>
#include "Item.h"

class ItemManager
{
private:
	ItemManager() = default;
public:
	static ItemManager* GetInst()
	{
		if (nullptr == m_inst)
			m_inst = new ItemManager();
		return m_inst;
	}
	static void DestroyInst()
	{
		SAFE_DELETE(m_inst);
	}

	void Init();
	void Update();
	void Render();
	void Clear();

	// 매 프레임 호출. 내부 타이머가 0이 되면 랜덤 타입을 랜덤 위치에 스폰한다.
	void TrySpawnItem(int groundLength);

private:
	int RandomSpawnDelayFrames() const;   // [MIN,MAX]초를 프레임으로 환산한 랜덤 값.

	static ItemManager* m_inst;
	std::vector<std::unique_ptr<Item>> m_items;
	int m_spawnTimer = 0;   // 다음 스폰까지 남은 프레임.
};
```

- [ ] **Step 2:** `ItemManager.cpp` 생성.

```cpp
#include "ItemManager.h"
#include "Defines.h"
#include <cstdlib>

ItemManager* ItemManager::m_inst = nullptr;

int ItemManager::RandomSpawnDelayFrames() const
{
	// [MIN, MAX]초 사이 랜덤 -> 프레임. FRAME=60.
	float span = ITEM_SPAWN_DELAY_MAX - ITEM_SPAWN_DELAY_MIN;
	float sec  = ITEM_SPAWN_DELAY_MIN + (std::rand() / (float)RAND_MAX) * span;
	return (int)(sec * FRAME);
}

void ItemManager::Init()
{
	m_items.clear();
	m_spawnTimer = RandomSpawnDelayFrames();
}

void ItemManager::TrySpawnItem(int groundLength)
{
	// 프레임 기반 카운트다운. 오버레이 중에는 이 함수가 안 불려 자동 정지된다.
	if (--m_spawnTimer > 0)
		return;

	m_spawnTimer = RandomSpawnDelayFrames();

	// 타입은 4종 균등 랜덤.
	ItemType type = (ItemType)(std::rand() % (int)ItemType::COUNT);

	// x는 적 스폰과 동일하게 지면 좌우 전체에서 랜덤(화면 중앙 +- groundLength).
	int span   = groundLength * 2;
	int spawnX = SCREEN_WIDTH / 2 - groundLength + (std::rand() % span);

	m_items.push_back(std::make_unique<Item>(type, Vector2{ spawnX, 0 }));
}

void ItemManager::Update()
{
	for (auto it = m_items.begin(); it != m_items.end();)
	{
		if (*it != nullptr)
		{
			(*it)->Tick();

			if ((*it)->IsLeaveDeadZone() || (*it)->IsDead())
			{
				(*it)->RemovePrevPos();
				it = m_items.erase(it);
				continue;
			}
		}
		++it;
	}
}

void ItemManager::Render()
{
	for (auto& item : m_items)
	{
		if (item != nullptr)
			item->Render();
	}
}

void ItemManager::Clear()
{
	m_items.clear();
	m_spawnTimer = 0;
}
```

- [ ] **Step 3:** .vcxproj에 `ItemManager.h`/`ItemManager.cpp` 추가.

- [ ] **Step 4 (검증):** Task 8(GameScene 배선) 후에 실제 스폰을 눈으로 확인한다. 지금은 컴파일만 통과 확인.

---

## Task 4: Bullet 클래스

**Files:**
- Create: `Bullet.h`
- Create: `Bullet.cpp`

**Interfaces:**
- Consumes: `ColliderTag::BULLET`/`ENEMY`/`TILE`(Task 1), `Collider`, `Enemy`, `Defines.h`.
- Produces:
  - `Bullet(Vector2 pos, int dir);`
  - `void Tick(); void Render() const; bool IsDead() const; void RemovePrevPos() const;`

- [ ] **Step 1:** `Bullet.h` 생성.

```cpp
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

	void RemovePrevPos() const;   // 제거 직전 잔상 지우기.

private:
	void OnCollision(Collider* other);

	Vector2 m_pos;
	int     m_dir;                 // +1 오른쪽, -1 왼쪽.
	float   m_accumX = 0.f;        // 정수 셀 이동을 위한 소수 누적.
	bool    m_isDead = false;

	std::unique_ptr<Collider> m_collider;

	mutable Vector2 m_lastRenderPos{ 0, 0 };
	mutable bool    m_hasLastRender = false;
};
```

- [ ] **Step 2:** `Bullet.cpp` 생성.

```cpp
#include "Bullet.h"
#include "ColliderManager.h"
#include "Enemy.h"
#include "Defines.h"

Bullet::Bullet(Vector2 pos, int dir)
	: m_pos(pos)
	, m_dir(dir)
	, m_collider(std::make_unique<Collider>(&m_pos, 1, 1, this, ColliderTag::BULLET))
{
	m_collider->SetOnCollision([this](Collider* c) { this->OnCollision(c); });
	ColliderManager::GetInst()->RegisterCollider(m_collider.get());
}

void Bullet::OnCollision(Collider* other)
{
	ColliderTag tag = other->GetTag();

	if (tag == ColliderTag::ENEMY)
	{
		Enemy* enemy = static_cast<Enemy*>(other->GetOwner());
		if (enemy && !enemy->IsDying())
			enemy->Kill();
		m_isDead = true;
		return;
	}

	if (tag == ColliderTag::TILE)
	{
		m_isDead = true;   // 벽에 맞으면 소멸(관통 X).
		return;
	}
	// PLAYER/ITEM/BULLET 등은 무시.
}

void Bullet::Tick()
{
	// 소수 속도를 누적해 정수 셀 단위로 이동.
	m_accumX += BULLET_SPEED;
	int steps = (int)m_accumX;
	if (steps != 0)
	{
		m_pos.x += m_dir * steps;
		m_accumX -= (float)steps;
	}

	// 화면 밖이면 소멸.
	if (m_pos.x < 0 || m_pos.x > SCREEN_WIDTH - 1)
		m_isDead = true;
}

void Bullet::RemovePrevPos() const
{
	if (!m_hasLastRender)
		return;

	SetColor();
	GotoXY(m_lastRenderPos.x, m_lastRenderPos.y);
	cout << "  ";
}

void Bullet::Render() const
{
	RemovePrevPos();

	SetColor(Color::CYAN);
	GotoXY(m_pos.x, m_pos.y);
	cout << "-";

	m_lastRenderPos = m_pos;
	m_hasLastRender = true;
}
```

- [ ] **Step 3:** .vcxproj에 `Bullet.h`/`Bullet.cpp` 추가.

- [ ] **Step 4 (검증):** 컴파일 통과 확인.

---

## Task 5: BulletManager 싱글턴

**Files:**
- Create: `BulletManager.h`
- Create: `BulletManager.cpp`

**Interfaces:**
- Consumes: `Bullet`(Task 4), `Utils.h`(SAFE_DELETE).
- Produces:
  - `static BulletManager* GetInst(); static void DestroyInst();`
  - `void Spawn(Vector2 pos, int dir); void Update(); void Render(); void Clear();`

- [ ] **Step 1:** `BulletManager.h` 생성.

```cpp
#pragma once
#include "Utils.h"
#include <vector>
#include <memory>
#include "Bullet.h"

class BulletManager
{
private:
	BulletManager() = default;
public:
	static BulletManager* GetInst()
	{
		if (nullptr == m_inst)
			m_inst = new BulletManager();
		return m_inst;
	}
	static void DestroyInst()
	{
		SAFE_DELETE(m_inst);
	}

	void Spawn(Vector2 pos, int dir);
	void Update();
	void Render();
	void Clear();

private:
	static BulletManager* m_inst;
	std::vector<std::unique_ptr<Bullet>> m_bullets;
};
```

- [ ] **Step 2:** `BulletManager.cpp` 생성.

```cpp
#include "BulletManager.h"

BulletManager* BulletManager::m_inst = nullptr;

void BulletManager::Spawn(Vector2 pos, int dir)
{
	m_bullets.push_back(std::make_unique<Bullet>(pos, dir));
}

void BulletManager::Update()
{
	for (auto it = m_bullets.begin(); it != m_bullets.end();)
	{
		if (*it != nullptr)
		{
			(*it)->Tick();

			if ((*it)->IsDead())
			{
				(*it)->RemovePrevPos();
				it = m_bullets.erase(it);
				continue;
			}
		}
		++it;
	}
}

void BulletManager::Render()
{
	for (auto& b : m_bullets)
	{
		if (b != nullptr)
			b->Render();
	}
}

void BulletManager::Clear()
{
	m_bullets.clear();
}
```

- [ ] **Step 3:** .vcxproj에 `BulletManager.h`/`BulletManager.cpp` 추가.

- [ ] **Step 4 (검증):** 컴파일 통과 확인.

---

## Task 6: EnemyManager::KillAll

**Files:**
- Modify: `EnemyManager.h:28` (public 메서드 선언 추가)
- Modify: `EnemyManager.cpp` (구현 추가)

**Interfaces:**
- Produces: `void EnemyManager::KillAll();`  // 살아있는 모든 적을 Kill().

- [ ] **Step 1:** `EnemyManager.h`의 `TrySpawnEnemyInRandomPos` 선언 아래(28번 줄 근처)에 추가.

```cpp
	void KillAll();   // 필드의 모든 적을 즉사시킨다(모두죽이기 아이템).
```

- [ ] **Step 2:** `EnemyManager.cpp` 끝에 구현 추가.

```cpp

void EnemyManager::KillAll()
{
	for (auto& enemy : m_enemys)
	{
		if (enemy != nullptr)
			enemy->Kill();   // Kill()이 enemy_death 사운드 재생 + 사망 처리.
	}
}
```

- [ ] **Step 3 (검증):** 컴파일 통과 확인.

---

## Task 7: Player 효과 통합

**Files:**
- Modify: `Player.h` (멤버/메서드 추가)
- Modify: `Player.cpp` (include, Tick, OnCollision, UpdateColor, Render, 신규 메서드)

**Interfaces:**
- Consumes: `Item`(GetType/ColorForType, Task 2), `BulletManager`(Spawn, Task 5), `EnemyManager`(KillAll, Task 6), `Defines.h` 상수.
- Produces: (내부 전용. 외부 인터페이스 변화 없음.)

- [ ] **Step 1:** `Player.h`에 멤버와 메서드를 추가한다. 기존 `private:` 멤버 블록(체력 위쪽, `m_chargedWheel` 근처)에 아래를 추가.

```cpp
	// ── 아이템 효과 상태(전부 프레임 카운터 → 오버레이 중 자동 정지) ──
	int m_starTimer = 0;   // 별 남은 프레임(>0이면 무적+접촉즉사+★).
	int m_gunTimer  = 0;   // 총 남은 프레임(>0이면 주기적으로 총알 발사).
	int m_gunFireCounter = 0;   // 다음 발사까지 남은 프레임.
	int m_facing = 1;      // 마지막 이동 방향(+1 오른쪽, -1 왼쪽), 기본 오른쪽.

	// 지속 틴트: 가장 최근에 먹은 지속 효과(별/총). HEAL은 '없음' 의미로 사용.
	ItemType m_lastPersistentPick = ItemType::HEAL;
```

그리고 `private:` 메서드 선언부(예: `UpdateInvincibility` 근처)에 추가.

```cpp
	void ApplyItem(ItemType type);    // 획득한 아이템 효과 적용.
	void UpdateItemEffects();         // facing/별/총 타이머 갱신 + 총 발사.
	ItemType CurrentTintType();       // 현재 표시할 지속 틴트 타입(가장 최근 활성).
```

`Player.h` 상단 include에 `#include "Item.h"` 추가(ItemType 사용).

- [ ] **Step 2:** `Player.cpp` 상단 include에 추가.

```cpp
#include "Item.h"
#include "BulletManager.h"
#include "EnemyManager.h"
```

- [ ] **Step 3:** `Player.cpp`의 `Tick()`에서 효과 갱신을 **두 반환 경로 모두**에 추가한다. 차징 분기의 `UpdateColor(); return;` 직전, 그리고 함수 끝의 `UpdateColor();` 직전에 각각 `UpdateItemEffects();`를 넣는다.

차징 분기(기존):
```cpp
			Pawn::Tick();
			UpdateInvincibility();
			UpdateCameraShake();
			UpdateColor();
			return;
```
수정:
```cpp
			Pawn::Tick();
			UpdateInvincibility();
			UpdateCameraShake();
			UpdateItemEffects();
			UpdateColor();
			return;
```

함수 끝(기존):
```cpp
	Pawn::Tick();
	UpdateInvincibility();
	UpdateCameraShake();
	UpdateColor();
}
```
수정:
```cpp
	Pawn::Tick();
	UpdateInvincibility();
	UpdateCameraShake();
	UpdateItemEffects();
	UpdateColor();
}
```

- [ ] **Step 4:** `Player.cpp`의 `OnCollision`에서 `ENEMY` 케이스를 별(STAR) 우선 처리로 바꾸고, `ITEM` 케이스를 추가한다. 기존 `if (tag == ColliderTag::ENEMY)` 블록 전체를 아래로 교체한다.

```cpp
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
```

- [ ] **Step 5:** `Player.cpp`에 신규 메서드들을 추가한다(파일 끝 근처, 적절한 위치).

```cpp
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

	// 별 타이머: 종료 시 원래 아이콘 복귀.
	if (m_starTimer > 0)
	{
		if (--m_starTimer == 0)
			m_renderIcon = "●";
	}

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
```

- [ ] **Step 6:** `Player.cpp`의 `UpdateColor()`를 지속 틴트 우선으로 수정한다.

```cpp
void Player::UpdateColor()
{
	// 지속 효과(별/총)가 켜져 있으면 그 아이템 색으로 틴트(가장 최근 것 우선).
	if (m_starTimer > 0 || m_gunTimer > 0)
	{
		m_renderColor = Item::ColorForType(CurrentTintType());
		return;
	}

	// 평상시: 적과 같은 색일 때 크리티컬이 나도록 색 판정용 속도로 색을 정한다.
	m_renderColor = Enemy::ColorForSpeed(ColorSpeed());
}
```

- [ ] **Step 7:** `Player.cpp`의 `Render()`에 별 종료 직전 깜빡임을 추가한다. 기존 무적 깜빡임 블록 **다음**, `Pawn::Render();` **앞**에 삽입한다.

```cpp
	// 별 종료 1초 전부터 깜빡여 곧 끝남을 알린다.
	if (m_starTimer > 0 && m_starTimer <= STAR_BLINK_WARN_FRAMES)
	{
		bool hide = ((m_starTimer / PLAYER_BLINK_INTERVAL) % 2) == 1;
		if (hide)
		{
			RemovePrevPos();
			m_hasLastRender = false;
			return;
		}
	}
```

- [ ] **Step 8 (검증):** 사용자가 빌드. 컴파일 통과 확인(실제 동작은 Task 8 배선 후).

---

## Task 8: GameScene 배선

**Files:**
- Modify: `GameScene.cpp` (include, Init, Update, Render, Release)

**Interfaces:**
- Consumes: `ItemManager`(Task 3), `BulletManager`(Task 5).

- [ ] **Step 1:** `GameScene.cpp` 상단 include에 추가.

```cpp
#include "ItemManager.h"
#include "BulletManager.h"
```

- [ ] **Step 2:** `Init()`의 `EnemyManager::GetInst()->Init();` 다음 줄에 추가.

```cpp
	ItemManager::GetInst()->Init();
```

- [ ] **Step 3:** `Update()`를 수정한다. 기존:
```cpp
void GameScene::Update()
{
	EnemyManager::GetInst()->TrySpawnEnemyInRandomPos(m_player->GetPos(), m_gameMap->GetGroundLength());

	m_player->Tick();
	EnemyManager::GetInst()->Update();
}
```
수정:
```cpp
void GameScene::Update()
{
	EnemyManager::GetInst()->TrySpawnEnemyInRandomPos(m_player->GetPos(), m_gameMap->GetGroundLength());
	ItemManager::GetInst()->TrySpawnItem(m_gameMap->GetGroundLength());

	m_player->Tick();
	EnemyManager::GetInst()->Update();
	ItemManager::GetInst()->Update();
	BulletManager::GetInst()->Update();
}
```

- [ ] **Step 4:** `Render()`에서 적 렌더 다음, 플레이어 렌더 앞에 아이템/총알 렌더를 넣는다. 기존:
```cpp
	EnemyManager::GetInst()->Render();
	m_player->Render();
```
수정:
```cpp
	EnemyManager::GetInst()->Render();
	ItemManager::GetInst()->Render();
	BulletManager::GetInst()->Render();
	m_player->Render();
```

- [ ] **Step 5:** `Release()`에 추가. 기존 `EnemyManager::GetInst()->Clear();` 다음 줄에.

```cpp
	ItemManager::GetInst()->Clear();
	BulletManager::GetInst()->Clear();
```

- [ ] **Step 6 (검증):** 사용자가 빌드 후 실행. 확인 항목:
  - 게임 중 6~12초마다 아이템이 떨어져 착지하고, 깜빡임이 끝으로 갈수록 빨라지다 사라진다.
  - `♥` 먹으면 HP 1 증가(최대 3 유지), `★` 먹으면 플레이어가 ★로 변하고 적과 닿으면 적이 죽고 플레이어는 안 죽음, 5초 후 끝나기 1초 전 깜빡이며 ●로 복귀.
  - `♠` 먹으면 마지막 이동 방향으로 0.6초마다 총알이 나가 적을 죽인다(벽/화면밖에서 소멸). 6초 지속.
  - `◑` 먹으면 화면의 모든 적이 즉사하고 화면이 강하게 흔들린다.
  - 별과 총을 둘 다 먹으면 둘 다 동작하고, 플레이어 색은 나중에 먹은 효과 색.

---

## Task 9: Core 배선(싱글턴 파괴 + 사운드)

**Files:**
- Modify: `Core.cpp` (생성자 사운드 Load, 소멸자 DestroyInst)

**Interfaces:**
- Consumes: `ItemManager`/`BulletManager` DestroyInst, `SoundManager`.

- [ ] **Step 1:** `Core.cpp` 상단 include에 추가.

```cpp
#include "ItemManager.h"
#include "BulletManager.h"
```

- [ ] **Step 2:** `Core` 생성자의 사운드 로드 블록(`SOUND->Load("critical", ...)` 다음)에 추가.

```cpp
	SOUND->Load("item_pickup", "Sounds/item_pickup.mp3");   // 아이템 획득.
	SOUND->Load("gun_fire",    "Sounds/gun_fire.mp3");      // 총 발사.
```

- [ ] **Step 3:** `Core::~Core`에서 `EnemyManager::GetInst()->DestroyInst();` 다음(그리고 `ColliderManager::DestroyInst()` **앞**)에 추가한다. 콜라이더를 들고 있으므로 ColliderManager보다 먼저 파괴해야 한다.

```cpp
	ItemManager::GetInst()->DestroyInst();
	BulletManager::GetInst()->DestroyInst();
```

- [ ] **Step 4:** `Sounds/` 폴더에 `item_pickup.mp3`, `gun_fire.mp3` 파일을 넣는다(사용자가 준비). 파일이 없어도 `SoundManager`가 로드 실패를 무시하면 게임은 정상 동작하나, 효과음만 안 난다. (필요 시 빈 mp3 또는 기존 파일 복사로 임시 대체.)

- [ ] **Step 5 (검증):** 사용자가 빌드 후 실행. 종료 시(타이틀 복귀/창 닫기) 크래시 없이 정리되는지 확인. 아이템 획득/발사 시 효과음 확인.

---

## Self-Review 결과

- **Spec 커버리지:** 낙하/착지(Task 2), 랜덤 스폰·동시 여러 개(Task 3), 점진 가속 깜빡임(Task 2), 무적 중 획득(Task 7-Step4), HEAL/STAR/GUN/KILL_ALL(Task 7-Step5), 별 외형·종료 깜빡임(Task 7 Step5/7), 총 방향·발사·벽소멸(Task 4/7), 지속 틴트·중첩(Task 7 Step6), 킬올 2배 흔들림(Task 7 Step5), 사운드(Task 9), 배선/정리(Task 8/9) — 모두 매핑됨.
- **Placeholder:** 없음. 모든 코드 완성.
- **타입 일관성:** `ItemType`(HEAL/STAR/GUN/KILL_ALL/COUNT), `ColorForType`, `GetType`, `Consume`, `KillAll`, `Spawn(Vector2,int)` 등 정의/사용 일치 확인.
- **알려진 리스크:** (1) 아이콘 글리프/한글 주석 인코딩(Global Constraints 참고). (2) 아이템이 가시(SPIKE) 위에 착지하면 `Pawn::OnCollision`이 즉사 처리해 바로 사라질 수 있음 — 드물고 허용 가능. (3) `SoundManager`가 누락 mp3를 어떻게 처리하는지 빌드 시 확인 필요.
