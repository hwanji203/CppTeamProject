#include "ColliderManager.h"
#include "Collider.h"
#include <algorithm>

ColliderManager* ColliderManager::m_pInst = nullptr;

ColliderManager* ColliderManager::GetInst()
{
	if (!m_pInst)
		m_pInst = new ColliderManager();
	return m_pInst;
}

void ColliderManager::DestroyInst()
{
	delete m_pInst;
	m_pInst = nullptr;
}

void ColliderManager::RegisterCollider(Collider* pCollider)
{
	m_colliders.push_back(pCollider);
}

void ColliderManager::UnregisterCollider(Collider* pCollider)
{
	auto it = std::find(m_colliders.begin(), m_colliders.end(), pCollider);
	if (it != m_colliders.end())
		m_colliders.erase(it);
}

void ColliderManager::Update()
{
	// O(n²) 전체 쌍 검사 — 오브젝트 수가 적은 콘솔 게임에서는 충분
	for (size_t i = 0; i < m_colliders.size(); ++i)
	{
		for (size_t j = i + 1; j < m_colliders.size(); ++j)
		{
			Collider* a = m_colliders[i];
			Collider* b = m_colliders[j];

			if (a->Overlaps(*b))
			{
				a->InvokeCollision(b);
				b->InvokeCollision(a);
			}
		}
	}
}
