#pragma once
#include <vector>
#include "Tile.h"
#include "Collider.h"

class GameMap
{
public:
    GameMap(int _w, int _h);
    ~GameMap();
public:
    void Fill(const Tile& _t);
    void Render() const;
    void SetupCollider();
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
    bool IsMergeable(int y, int x, Tile::Type cat, int maxX, std::vector<std::vector<bool>>& visited)
    {
        return x >= 0 && x < maxX && y >= 0 && y < this->m_height
            && !visited[y][x] && m_vecMapData[y][x].blocked
            && m_vecMapData[y][x].type == cat;
    }
private:
    int m_width;
    int m_height;
    std::vector<std::vector<Tile>> m_vecMapData;
    std::vector<Collider> m_colliders;

    // Collider�� ������ Vector2 ��ü���� �޸𸮿��� ������� �ʵ��� �����ϴ� ����Դϴ�.
    std::vector<Vector2> m_colliderPositions;
};