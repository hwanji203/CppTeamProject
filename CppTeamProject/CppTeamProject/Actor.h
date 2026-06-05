#pragma once
#include "Position.h"
class Actor
{
public:
	Actor(Position _pos = { 0,0 });
	virtual ~Actor() = default;

	virtual void Tick() abstract;
	virtual void Render() const abstract;

public:
	// inline 함수 = 매크로 + 함수의 장점
	const Position& GetPos() const
	{
		return m_pos;
	}
protected:
	Position m_pos;
	Position m_prevPos;
};

