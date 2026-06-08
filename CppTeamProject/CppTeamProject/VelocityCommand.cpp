#include "VelocityCommand.h"
#include "Actor.h"
#include "IPhysicsable.h"
#include "Pawn.h"

VelocityCommand::VelocityCommand()
	: m_velocityX(0)
{
}

VelocityCommand::VelocityCommand(int delta)
	: m_velocityX(delta)
{
}

void VelocityCommand::Execute(Actor* _actor)
{
	IPhysicsable* pawn = dynamic_cast<IPhysicsable*>(_actor);
	// 이 actor가 어떤애냐에 따라서 뭘?
	if (pawn != nullptr)
		pawn->SetVelocity(m_velocityX);
}
