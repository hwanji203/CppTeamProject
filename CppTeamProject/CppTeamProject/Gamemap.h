#pragma once
#include<vector>
#include "Tile.h"
class GameMap
{
public:
	GameMap(int _w, int _h);
public:
	void Fill(const Tile& _t);
	void Render(const Position& _playerPos) const;
public:
	void SetTile(int _x, int _y, Tile _t)
	{
		m_vecMapData[_y][_x] = _t;
	}
	const Tile& GetTile(int _x, int _y) const
	{
		return m_vecMapData[_y][_x];
	}
	const Tile::Type& GetTileType(int _x, int _y) const
	{
		return m_vecMapData[_y][_x].type;
	}
private:
	int m_width;
	int m_height;
	std::vector<std::vector<Tile>> m_vecMapData;
};

