#pragma once
#include "Actor.h"
#include "IPhysicsable.h"
#include "Rigidbody.h"
#include "Collider.h"
#include "Defines.h"
#include <memory>

class Pawn : public Actor, public IPhysicsable
{
public:
	Pawn(Vector2 _pos = { 0,0 }, std::string renderIcon = "§", Vector2 deadZone = {SCREEN_WIDTH, SCREEN_HEIGHT}, ColliderTag tag = ColliderTag::PLAYER);
	~Pawn();

	virtual void Tick() override;
	virtual void Render() const override;

	Rigidbody* GetRigidbody() const { return m_rigidbody.get(); }
	Collider*  GetCollider()  const { return m_collider.get(); }

	// IPhysicsable��(��) ���� ��ӵ�
	void SetVelocity(float velocityX) override;
	bool IsLeaveDeadZone();
	bool IsDead() const { return m_isDead; }

protected:
	void RemovePrevPos() const;

private:
	void OnCollision(Collider* other);

protected:
	std::unique_ptr<Rigidbody> m_rigidbody;
	std::unique_ptr<Collider>  m_collider;

private:
	Vector2 m_deadZone;
	bool    m_isDead = false;

	// 직전에 그린 위치 1칸만 기억해 다음 프레임에 지움 (trail 무한 증가/깜빡임 방지)
	mutable Vector2 m_lastRenderPos{ 0, 0 };
	mutable bool    m_hasLastRender = false;
};

struct test
{
	int a = 0;
};

struct test2
{
	int a = 0;
};