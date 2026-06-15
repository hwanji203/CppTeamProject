#include "Player.h"
#include "Console.h"
#include "Defines.h"

Player::Player(Vector2 pos)
	: Pawn(pos, Color::CYAN, "¡Ü", { SCREEN_WIDTH, SCREEN_HEIGHT }, ColliderTag::PLAYER),
	m_canMove(true)
{
}

void Player::Tick()
{
	if (m_rigidbody->IsGrounded())
	{
		if (GetKey(VK_XBUTTON1))
		{
			m_rigidbody->SetFrozen(true);

			int wheel = GetMouseWheelChange();
			if (wheel != 0)
				m_velocityStack.push(wheel);

			Pawn::Tick();
			return;
		}

		if (m_rigidbody->IsFrozen())
		{
			m_rigidbody->SetFrozen(false);

			int total = 0;
			while (!m_velocityStack.empty())
			{
				total += m_velocityStack.top();
				m_velocityStack.pop();
			}
			SetVelocity(total * PLAYER_SPEED);
		}

		SetVelocity(GetMouseWheelChange() * PLAYER_SPEED);

		if (GetKey('a') || GetKey('A'))
			SetVelocity(-1 * PLAYER_KEYBOARD_SPEED);
		if (GetKey('d') || GetKey('D'))
			SetVelocity(1 * PLAYER_KEYBOARD_SPEED);
	}

	Pawn::Tick();
}
