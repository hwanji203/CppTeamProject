#pragma once
#include <vector>

class Collider;
enum class ColliderTag;

class ColliderManager
{
public:
	static ColliderManager* GetInst();
	static void             DestroyInst();

	void RegisterCollider(Collider* pCollider);
	void UnregisterCollider(Collider* pCollider);

	void Update();

	// self를 제외하고, 주어진 태그를 가진 콜라이더와 현재 겹쳐 있으면 true.
	bool IsOverlappingTag(Collider* self, ColliderTag tag) const;

	// self와 겹친 첫 번째 (해당 태그) 콜라이더를 반환. 없으면 nullptr.
	Collider* FindOverlappingTag(Collider* self, ColliderTag tag) const;

private:
	ColliderManager()  = default;
	~ColliderManager() = default;

	static ColliderManager* m_pInst;

	std::vector<Collider*> m_colliders;
};