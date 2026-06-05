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