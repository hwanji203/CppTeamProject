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

}

void GameMap::Fill(const Tile& _t)
{
	// �ݺ���
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

			// Ÿ���� ���
			const Tile& tile = m_vecMapData[y][x];
			auto [textColor, bgColor] = tile.color;
			SetColor(textColor, bgColor);
			cout << tile.symbol; // ���� ����
		}
		//cout << endl;
	}
}

void GameMap::SetupCollider()
{
    m_colliders.clear();
    m_colliderPositions.clear();

    int maxX = m_width / 2 - 1;

    // 포인터 무효화 방지: 최대 blocked 타일 수만큼 예약 (재할당 시 등록된 포인터 무효화 방지)
    m_colliderPositions.reserve(m_height * maxX);
    m_colliders.reserve(m_height * maxX);

    std::vector<std::vector<bool>> visited(m_height, std::vector<bool>(maxX, false));

    for (int y = 0; y < m_height; ++y)
    {
        for (int x = 0; x < maxX; ++x)
        {
            if (!m_vecMapData[y][x].blocked || visited[y][x])
                continue;

            // BFS: 같은 행에서 좌우로만 확장
            std::queue<int> q;
            q.push(x);
            visited[y][x] = true;

            int minX = x, maxRunX = x;

            while (!q.empty())
            {
                int cx = q.front(); q.pop();

                for (int dx : {-1, 1})
                {
                    int nx = cx + dx;
                    if (nx >= 0 && nx < maxX && !visited[y][nx] && m_vecMapData[y][nx].blocked)
                    {
                        visited[y][nx] = true;
                        q.push(nx);
                        minX = std::min(minX, nx);
                        maxRunX = std::max(maxRunX, nx);
                    }
                }
            }

            // 짝수 길이 run은 오른쪽 1칸 오차 허용 (option B)
            // 타일 → screen 좌표 변환 (x * 2)
            int center_x = (minX + maxRunX) / 2 * 2;
            int halfWidth = (center_x / 2 - minX) * 2;

            m_colliderPositions.push_back(Vector2{ center_x, y });
            Vector2* pPos = &m_colliderPositions.back();
            m_colliders.push_back(Collider(pPos, halfWidth, this));
            ColliderManager::GetInst()->RegisterCollider(&m_colliders.back());
        }
    }
}
