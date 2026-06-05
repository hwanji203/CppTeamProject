#pragma once
// Utils.h 
// Uils.h
#include <random>
#include <concepts>
#include "Position.h"   // ← 직접 include 추가
namespace random_engine
{
	// c++17? inline 변수
	inline std::mt19937 mt19937(std::random_device{}());
}

template<typename T>
T RandomInRange(std::mt19937& _e, T _low, T _high) requires std::integral<T>
{
	std::uniform_int_distribution<T> dist(_low, _high);
	return dist(_e);
}

inline int DistanceSquared(const Position& _a, const Position& _b)
{
	int dx = _b.x - _a.x;
	int dy = _b.y - _a.y;
	return dx * dx + dy * dy;
}