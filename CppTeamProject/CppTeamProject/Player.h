#pragma once
#include "Pawn.h"
#include<stack>

// 입력(마우스 휠)으로 조작되는 플레이어.
// 입력 처리를 Pawn에서 분리해, Enemy는 더 이상 입력을 받지 않는다.
class Player : public Pawn
{
public:
	Player(Vector2 pos = { 0, 0 });

	virtual void Tick() override;

private:
	std::stack<int> m_velocityStack;
	bool m_canMove;
	ULONGLONG m_nextCanMove;
};
