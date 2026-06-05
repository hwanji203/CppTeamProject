#include "GameMap.h"

GameMap::GameMap(int _w, int _h)
	: m_width(_w)
	, m_height(_h)
	, m_vecMapData(_h, std::vector<Tile>(_w, Tile::Type::EMPTY))
{
}

void GameMap::Fill(const Tile& _t)
{
	// 반복자
	for (auto& row : m_vecMapData)
	{
		std::fill(row.begin(), row.end(), _t);
	}
}

void GameMap::Render(const Position& _playerPos) const
{
	for (int y = 0; y < m_height; ++y)
	{
		for (int x = 0; x < m_width; ++x)
		{
			GotoXY(x * 2, y);
			if (_playerPos == Position{ x,y })
				continue;

			// 타일을 찍기
			const Tile& tile = m_vecMapData[y][x];
			auto [textColor, bgColor] = tile.color;
			SetColor(textColor, bgColor);
			cout << tile.symbol; // 블랙 색깔
		}
		//cout << endl;
	}

}
