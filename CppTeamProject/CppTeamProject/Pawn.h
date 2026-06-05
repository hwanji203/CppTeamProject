#pragma once
#include "Actor.h"
#include "IMovable.h"
class Pawn : public Actor, public IMovable
{
public:
	Pawn(Position _pos = { 0,0 });

	// IMovable을(를) 통해 상속됨
	void Move(Dir _dir) override;

	// Actor을(를) 통해 상속됨
	void Tick() override;
	void Render() const override;
};