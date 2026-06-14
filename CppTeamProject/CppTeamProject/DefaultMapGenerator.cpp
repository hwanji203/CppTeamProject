#include "DefaultMapGenerator.h"

std::unique_ptr<GameMap> DefaultMapGenerator::Generate(int _width, int _height)
{
    m_groundY = _height * 3 / 4;   // 가운데 바닥 높이

    auto map = std::make_unique<GameMap>(_width, _height);

    // 1. 전체를 빈 공간(EMPTY)으로 채움
    map->Fill(Tile::Type::EMPTY);

    const int maxX       = _width / 2 - 1;   // 렌더/콜라이더가 사용하는 타일 열 수
    const int leftWallX  = 0;
    const int rightWallX = maxX - 1;
    const int bottomY    = _height - 1;

    // 2. 좌우 세로벽 (천장 ~ 바닥)
    for (int y = 0; y < _height; ++y)
    {
        map->SetTile(leftWallX,  y, Tile::Type::GROUND);
        map->SetTile(rightWallX, y, Tile::Type::GROUND);
    }

    // 3. 가운데 바닥 한 줄 (좌우 벽과 사이에 떨어질 틈을 남김)
    const int floorStart = maxX / 4;
    const int floorEnd   = maxX * 3 / 4;
    for (int x = floorStart; x <= floorEnd; ++x)
    {
        map->SetTile(x, m_groundY, Tile::Type::GROUND);
    }

    // 4. 맨 아래줄 가시 (좌우 벽 사이) - 떨어지면 사망
    for (int x = leftWallX + 1; x < rightWallX; ++x)
    {
        map->SetTile(x, bottomY, Tile::Type::SPIKE);
    }

    map->SetupCollider();

    return map;
}
