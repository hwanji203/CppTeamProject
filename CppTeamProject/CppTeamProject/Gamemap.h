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
    void Render(const Vector2& _playerPos) const;
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
private:
    int m_width;
    int m_height;
    std::vector<std::vector<Tile>> m_vecMapData;
    std::vector<Collider> m_colliders;

    // Collider가 참조할 Vector2 객체들이 메모리에서 사라지지 않도록 보관하는 장소입니다.
    std::vector<Vector2> m_colliderPositions;
};