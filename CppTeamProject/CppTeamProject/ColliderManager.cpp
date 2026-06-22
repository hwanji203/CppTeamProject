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

bool ColliderManager::IsOverlappingTag(Collider* self, ColliderTag tag) const
{
	return FindOverlappingTag(self, tag) != nullptr;
}

Collider* ColliderManager::FindOverlappingTag(Collider* self, ColliderTag tag) const
{
	for (Collider* c : m_colliders)
	{
		if (c == self)
			continue;
		if (c->GetTag() == tag && self->Overlaps(*c))
			return c;
	}
	return nullptr;
}
