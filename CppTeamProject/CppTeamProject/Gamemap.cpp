#include "GameMap.h"
#include "ColliderManager.h"

GameMap::GameMap(int _w, int _h)
	: m_width(_w)
	, m_height(_h)
	, m_vecMapData(_h, std::vector<Tile>(_w, Tile::Type::EMPTY))
{

}

GameMap::~GameMap()
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

void GameMap::Render(const Vector2& _playerPos) const
{
	for (int y = 0; y < m_height; ++y)
	{
		for (int x = 0; x < m_width / 2 - 1; ++x)
		{
			GotoXY(x * 2, y);
			if (_playerPos == Vector2{ x,y })
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

void GameMap::SetupCollider()
{
    m_colliders.clear();
    m_colliderPositions.clear();

    // Render 함수에서 사용하시는 맵의 실제 x축 범위를 동일하게 적용합니다.
    int maxX = m_width / 2 - 1;

    // std::vector의 재할당으로 인해 포인터 주소가 바뀌는 것을 막기 위해
    // 발생할 수 있는 최대 벽의 개수만큼 미리 메모리 공간을 예약(reserve)합니다.
    m_colliderPositions.reserve(m_height * maxX);

    for (int y = 0; y < m_height; ++y)
    {
        for (int x = 0; x < maxX; ++x)
        {
            if (m_vecMapData[y][x].blocked)
            {
                // 1. 메모리에 안전하게 유지될 Vector2 객체를 생성하여 보관합니다.
                m_colliderPositions.push_back(Vector2{ x, y });

                // 2. 방금 보관한 Vector2 객체의 실제 메모리 주소(&)를 가져옵니다.
                Vector2* pPos = &m_colliderPositions.back();

                // 3. 타일 1개의 폭을 차지하므로, 중심점 기준 좌우 확장값(halfWidth)은 0입니다.
                // pOwner로는 현재 GameMap 자신(this)을 넘겨줍니다.
                Collider newCollider(pPos, 0, this);
                m_colliders.push_back(newCollider);
            }
        }
    }
}