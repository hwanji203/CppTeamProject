#pragma once
#include <vector>

class Collider;

class ColliderManager
{
public:
	static ColliderManager* GetInst();
	static void             DestroyInst();

	void RegisterCollider(Collider* pCollider);
	void UnregisterCollider(Collider* pCollider);

	void Update();

private:
	ColliderManager()  = default;
	~ColliderManager() = default;

	static ColliderManager* m_pInst;

	std::vector<Collider*> m_colliders;
};
