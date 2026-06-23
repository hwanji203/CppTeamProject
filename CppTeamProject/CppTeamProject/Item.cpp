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
