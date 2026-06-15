#pragma once
#include "Console.h"
#include "Vector2.h"
#include<vector>
#include<string>
#include<memory>

class Actor
{
public:
	Actor(Vector2 _pos = { 0,0 }, Color renderColor = Color::CYAN, std::string renderIcon = ".");
	virtual ~Actor() = default;

	virtual void Tick() abstract;
	virtual void Render() const abstract;

public:
	// inline 함수 = 매크로 + 함수의 장점
	const Vector2* GetPos() const
	{
		return &m_pos;
	}
protected:
	Vector2 m_pos;
	std::unique_ptr<std::vector<Vector2>> m_prevPoses;
	std::string m_renderIcon;
	Color m_renderColor;
};

