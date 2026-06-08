#pragma once
#include "ICommand.h"
#include "Enums.h"
class VelocityCommand : public ICommand
{
public:
	VelocityCommand();
	VelocityCommand(int delta);
	void Execute(Actor* _actor) override;
private:
	float m_velocityX;
};

