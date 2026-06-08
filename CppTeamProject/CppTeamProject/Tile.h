#pragma once
// ��, �ٴ�, ��, ���
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
				symbol = "��"; // ������ 2��°�� 6��
				color.first = Color::LIGHT_GRAY;
				break;
			case Tile::Type::FLOOR:
				symbol = "��"; // ������ 2��°�� 8��
				color.first = Color::GRAY;
				break;
			case Tile::Type::DOOR:
				symbol = "��"; // ������ 2��°�� 7��
				color.first = Color::LIGHT_RED;
				break;
			case Tile::Type::STAIRS:
				symbol = "��"; // ������ 5��°�� 6��
				color.first = Color::GRAY;
				break;
		}
	}
};