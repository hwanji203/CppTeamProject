#pragma once
#include "VelocityCommand.h"
#include "Enums.h"
#include<map>

class CommandPool
{
public:
	CommandPool();
public:
	ICommand* GetVelocityCommand(int delta)
	{
		return &(m_velocityCommand[delta]);
	}
private:
	std::map<int, VelocityCommand> m_velocityCommand;
};

