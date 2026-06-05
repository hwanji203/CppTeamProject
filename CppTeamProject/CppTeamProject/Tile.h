#pragma once
// 벽, 바닥, 문, 계단
#include "Console.h"
struct Tile
{
	enum class Type { EMPTY, GROUND, END } type;
	bool blocked;
	std::string symbol;
	std::pair<Color, Color> color;
	Tile(Type t = Type::EMPTY,
		 Color textColor = Color::WHITE,
		 Color bgColor = Color::BLACK)
		: type(t)
		, blocked(t == Type::GROUND)
		, symbol("")
		, color(textColor, bgColor)
	{
		switch (t)
		{
			case Tile::Type::EMPTY:
				symbol = "  ";
				color.first = Color::BLACK;
				break;
			case Tile::Type::GROUND:
				symbol = "■"; // ㅁ한자 2번째줄 6번
				color.first = Color::LIGHT_GRAY;
				break;
		}
	}
};