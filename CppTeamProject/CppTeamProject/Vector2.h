#pragma once
struct Vector2
{
	int x;
	int y;
	bool operator ==(const Vector2& _other)
	{
		return x == _other.x && y == _other.y;
	}
	Vector2 operator +(const Vector2& _other) const
	{
		return { _other.x + x, _other.y + y };
	}
};