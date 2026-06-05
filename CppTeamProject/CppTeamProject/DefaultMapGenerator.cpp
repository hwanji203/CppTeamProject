#include "DefaultMapGenerator.h"

std::unique_ptr<GameMap> DefaultMapGenerator::Generate(int _width, int _height)
{
    auto map = std::make_unique<GameMap>(_width, _height);

    // 1. 맵 전체를 빈 공간(EMPTY)으로 채움
    map->Fill(Tile::Type::EMPTY);

    // 2. 바닥(GROUND)을 가로로 길게 깖
    //    - 세로 위치: 화면 아래쪽 (높이의 3/4 지점)
    //    - 양쪽 끝은 비워둠 → 끝으로 가면 떨어짐
    int groundY = _height * 3 / 4;   // 바닥 높이 (아래쪽)
    int edgeGap = 2;                 // 양쪽 끝에 비워둘 칸 수

    for (int x = edgeGap; x < _width - edgeGap; ++x)
    {
        map->SetTile(x, groundY, Tile::Type::GROUND);
    }

    return map;
}