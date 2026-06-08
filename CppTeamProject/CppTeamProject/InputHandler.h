#pragma once
#include<vector>
#include<memory>
#include "Enums.h"
#include "CommandPool.h"
#include<map>
class ICommand;

class InputHandler
{
public:
	InputHandler();
public:
	ICommand* HandleInput();
private:
	std::map<int, Dir> m_vecKeys;
	CommandPool m_commandPool;
};

