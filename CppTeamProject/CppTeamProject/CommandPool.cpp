#include "CommandPool.h"

CommandPool::CommandPool()
{
	m_moveCommand[(int)Dir::UP] = MoveCommand(Dir::UP);
	m_moveCommand[(int)Dir::DOWN] = MoveCommand(Dir::DOWN);
	m_moveCommand[(int)Dir::LEFT] = MoveCommand(Dir::LEFT);
	m_moveCommand[(int)Dir::RIGHT] = MoveCommand(Dir::RIGHT);
}
