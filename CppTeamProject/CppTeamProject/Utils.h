#pragma once
// Utils.h 
// Uils.h
#include <random>
#include <concepts>
#include "Vector2.h"   // ← 직접 include 추가

#define SAFE_DELETE(p) if(p) { delete p; p=nullptr;}

inline int DistanceSquared(const Vector2& _a, const Vector2& _b)
{
	int dx = _b.x - _a.x;
	int dy = _b.y - _a.y;
	return dx * dx + dy * dy;
}