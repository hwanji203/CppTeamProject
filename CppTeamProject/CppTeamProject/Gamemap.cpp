#include "GameMap.h"
#include "ColliderManager.h"
#include <queue>
#include <algorithm>

GameMap::GameMap(int _w, int _h)
	: m_width(_w)
	, m_height(_h)
	, m_vecMapData(_h, std::vector<Tile>(_w, Tile::Type::EMPTY))
{

}

GameMap::~GameMap()
{
    for (auto& collider : m_colliders)
        ColliderManager::GetInst()->UnregisterCollider(&collider);

    m_colliders.clear();
    m_colliderPositions.clear();
}

void GameMap::Fill(const Tile& _t)
{
	for (auto& row : m_vecMapData)
	{
		std::fill(row.begin(), row.end(), _t);
	}
}

void GameMap::Render() const
{
	for (int y = 0; y < m_height; ++y)
	{
		for (int x = 0; x < m_width / 2 - 1; ++x)
		{
			GotoXY(x * 2, y);

			const Tile& tile = m_vecMapData[y][x];
            if (tile.type == Tile::Type::EMPTY)
                continue;
			auto [textColor, bgColor] = tile.color;
			SetColor(textColor, bgColor);
			cout << tile.symbol; // ���� ����
		}
	}
}

void GameMap::SetupCollider()
{
    m_colliders.clear();
    m_colliderPositions.clear();

    int maxX = m_width / 2 - 1;

    m_colliderPositions.reserve(m_height * maxX);
    m_colliders.reserve(m_height * maxX);

    std::vector<std::vector<bool>> visited(m_height, std::vector<bool>(maxX, false));

    for (int y = 0; y < m_height; ++y)
    {
        for (int x = 0; x < maxX; ++x)
        {
            if (!m_vecMapData[y][x].blocked || visited[y][x])
                continue;

            Tile::Type cat = m_vecMapData[y][x].type;

            int w = 1;
            while (IsMergeable(y, x + w, cat, maxX, visited))
                ++w;

            int h = 1;
            bool canExpand = true;
            while (y + h < m_height && canExpand)
            {
                for (int k = x; k < x + w; ++k)
                {
                    if (!IsMergeable(y + h, k, cat, maxX, visited))
                    {
                        canExpand = false;
                        break;
                    }
                }
                if (canExpand)
                    ++h;
            }

            for (int ry = y; ry < y + h; ++ry)
                for (int k = x; k < x + w; ++k)
                    visited[ry][k] = true;

            int screenLeft  = x * 2;
            int screenWidth = w * 2;

            ColliderTag tag = (cat == Tile::Type::SPIKE) ? ColliderTag::SPIKE : ColliderTag::TILE;

            m_colliderPositions.push_back(Vector2{ screenLeft, y });
            Vector2* pPos = &m_colliderPositions.back();
            m_colliders.push_back(Collider(pPos, screenWidth, h, this, tag));
            ColliderManager::GetInst()->RegisterCollider(&m_colliders.back());
        }
    }
}
