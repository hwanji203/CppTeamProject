#pragma once
//#include "Actor.h"
class Actor;
class ICommand
{
public:
	virtual ~ICommand() = default;
	virtual void Execute(Actor* _actor) abstract;
	//int*;
	//Actor*;
	//float*;
	//// 1바이트 = 8비트
};

