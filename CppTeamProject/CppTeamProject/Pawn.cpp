#include "Pawn.h"
#include "Console.h"
Pawn::Pawn(Position _pos)
	: Actor(_pos)
{

}

void Pawn::Move(Dir _dir)
{
	m_prevPos = m_pos;
	switch (_dir)
	{
		case Dir::UP:
			--m_pos.y;
			break;
		case Dir::DOWN:
			++m_pos.y;
			break;
		case Dir::LEFT:
			m_pos.x -= 1;
			break;
		case Dir::RIGHT:
			m_pos.x += 1;
			break;
	}
}

void Pawn::Render() const
{
	SetColor(Color::SKYBLUE);
	GotoXY(m_pos.x * 2, m_pos.y);
	cout << "§";
}

// 입력 -> 포지션 변경
void Pawn::Tick()
{
	// Pawn이 자체적으로 할거를 update돌리면 됨.
}

