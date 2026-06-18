#include "DefaultMapGenerator.h"

std::unique_ptr<GameMap> DefaultMapGenerator::Generate(int _width, int _height)
{
    m_groundY = _height * 3 / 4;

    auto map = std::make_unique<GameMap>(_width, _height);

    map->Fill(Tile::Type::EMPTY);

    const int maxX       = _width / 2 - 1;
    const int leftWallX  = 0;
    const int rightWallX = maxX - 1;
    const int bottomY    = _height - 1;

    for (int y = 0; y < _height; ++y)
    {
        map->SetTile(leftWallX,  y, Tile::Type::GROUND);
        map->SetTile(rightWallX, y, Tile::Type::GROUND);
    }

    const int floorStart = maxX / 4;
    const int floorEnd   = maxX * 3 / 4;
    map->SetGroundLength(floorEnd - floorStart);
    for (int x = floorStart; x <= floorEnd; ++x)
    {
        map->SetTile(x, m_groundY, Tile::Type::GROUND);
    }

    // 4. �� �Ʒ��� ���� (�¿� �� ����) - �������� ���
    for (int x = leftWallX + 1; x < rightWallX; ++x)
    {
        map->SetTile(x, bottomY, Tile::Type::SPIKE);
    }

    map->SetupCollider();

    return map;
}
