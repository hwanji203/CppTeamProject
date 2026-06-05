#pragma once
#include "ICommand.h"
#include "Enums.h"
class MoveCommand : public ICommand
{
public:
	MoveCommand();
	MoveCommand(Dir _dir);
	// ICommand을(를) 통해 상속됨
	void Execute(Actor* _actor) override;
private:
	Dir m_dir;
};

