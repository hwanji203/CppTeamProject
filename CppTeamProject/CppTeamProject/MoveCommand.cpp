#include "MoveCommand.h"
#include "Actor.h"
#include "IMovable.h"
MoveCommand::MoveCommand()
	: m_dir(Dir::NONE)
{
}
MoveCommand::MoveCommand(Dir _dir)
	: m_dir(_dir)
{
}

void MoveCommand::Execute(Actor* _actor)
{
	// actor중에 movable이라는 것을 상속받았으면 move 허용
	IMovable* mover = dynamic_cast<IMovable*>(_actor);
	// 이 actor가 어떤애냐에 따라서 뭘?
	if (mover != nullptr)
		mover->Move(m_dir);
}
