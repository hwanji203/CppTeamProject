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
