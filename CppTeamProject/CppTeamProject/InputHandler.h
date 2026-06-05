#pragma once
#include<vector>
#include<memory>
#include "Enums.h"
//#include "CommandPool.h"
class ICommand;
//#include "ICommand.h"
struct InputKey
{
	int vk;
	Dir dir;
	bool isPrevDown;
	int prevTime;
};
class InputHandler
{
public:
	InputHandler();
public:
	ICommand* HandleInput();
	//std::unique_ptr<ICommand> HandleInput();
private:
	std::vector<InputKey> m_vecKeys;
	//CommandPool m_commandPool;
};

