#pragma once
struct Position
{
	int x;
	int y;
	bool operator ==(const Position& _other)
	{
		return x == _other.x && y == _other.y;
	}
};