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
	ICommand* HandleInput();
private:
	CommandPool m_commandPool;
};

