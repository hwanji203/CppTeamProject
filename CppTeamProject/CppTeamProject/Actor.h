#pragma once
#include "Vector2.h"
class Actor
{
public:
	Actor(Vector2 _pos = { 0,0 });
	virtual ~Actor() = default;

	virtual void Tick() abstract;
	virtual void Render() const abstract;

public:
	// inline 함수 = 매크로 + 함수의 장점
	const Vector2& GetPos() const
	{
		return m_pos;
	}
protected:
	Vector2 m_pos;
	Vector2 m_prevPos;
};

