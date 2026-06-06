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

	// 매 프레임 SceneManager::Update() 이후 호출
	void Update();

private:
	ColliderManager()  = default;
	~ColliderManager() = default;

	static ColliderManager* m_pInst;

	std::vector<Collider*> m_colliders;
};