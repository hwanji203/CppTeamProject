#pragma once
// 벽, 바닥, 문, 계단
#include "Console.h"
struct Tile
{
	enum class Type { EMPTY, WALL, FLOOR, DOOR, STAIRS, END } type;
	bool blocked;
	std::string symbol;
	std::pair<Color, Color> color;
	Tile(Type t = Type::EMPTY,
		 Color textColor = Color::WHITE,
		 Color bgColor = Color::BLACK)
		: type(t)
		, blocked(t == Type::WALL || t == Type::EMPTY)
		, symbol("")
		, color(textColor, bgColor)
	{
		switch (t)
		{
			case Tile::Type::EMPTY:
				symbol = "  ";
				color.first = Color::BLACK;
				break;
			case Tile::Type::WALL:
				symbol = "■"; // ㅁ한자 2번째줄 6번
				color.first = Color::LIGHT_GRAY;
				break;
			case Tile::Type::FLOOR:
				symbol = "·"; // ㄱ한자 2번째줄 8번
				color.first = Color::GRAY;
				break;
			case Tile::Type::DOOR:
				symbol = "Π"; // ㅎ한자 2번째줄 7번
				color.first = Color::LIGHT_RED;
				break;
			case Tile::Type::STAIRS:
				symbol = "▤"; // ㅁ한자 5번째줄 6번
				color.first = Color::GRAY;
				break;
		}
	}
};