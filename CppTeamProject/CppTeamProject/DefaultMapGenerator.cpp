#include "DefaultMapGenerator.h"

std::unique_ptr<GameMap> DefaultMapGenerator::Generate(int _width, int _height)
{
    m_groundY = _height * 3 / 4;   // �ٴ� ���� (�Ʒ���)
    m_edgeGap = _height * 8 / 10;  // ���� ���� ����� ĭ ��

    auto map = std::make_unique<GameMap>(_width, _height);

    // 1. �� ��ü�� �� ����(EMPTY)���� ä��
    map->Fill(Tile::Type::EMPTY);

    // 2. �ٴ�(GROUND)�� ���η� ��� ��
    //    - ���� ��ġ: ȭ�� �Ʒ��� (������ 3/4 ����)
    //    - ���� ���� ����� �� ������ ���� ������

    for (int x = m_edgeGap; x < _width / 2 - m_edgeGap; ++x)
    {
        map->SetTile(x, m_groundY, Tile::Type::GROUND);
    }

    map->SetupCollider();

    return map;
}