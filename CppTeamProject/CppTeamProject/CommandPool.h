#pragma once
#include "MoveCommand.h"
#include "Enums.h"
class CommandPool
{
public:
	CommandPool();
public:
	ICommand* GetMoveCommand(Dir _dir)
	{
		return &m_moveCommand[(int)_dir];
	}
private:
	MoveCommand m_moveCommand[(int)Dir::END];
};

