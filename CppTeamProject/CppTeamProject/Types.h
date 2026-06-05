#pragma once
#include "Position.h"

struct Rect
{
	int x, y;// 좌상단 좌표
	int width, height; // 너비와 높이

	Position Center() const
	{
		int cx = x + width / 2;
		int cy = y + height / 2;
		return { cx, cy };
	}
};