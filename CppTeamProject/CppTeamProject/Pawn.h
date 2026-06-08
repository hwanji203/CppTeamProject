#pragma once
#include "Actor.h"
#include "IPhysicsable.h"
#include "Rigidbody.h"
#include "Collider.h"
#include <memory>

class Pawn : public Actor, public IPhysicsable
{
public:
	Pawn(Vector2 _pos = { 0,0 });
	~Pawn();

	virtual void Tick() override;
	virtual void Render() const override;

	Rigidbody* GetRigidbody() const { return m_rigidbody.get(); }
	Collider*  GetCollider()  const { return m_collider.get(); }

	// IPhysicsable을(를) 통해 상속됨
	void SetVelocity(float velocityX) override;

protected:
	std::unique_ptr<Rigidbody> m_rigidbody;
	std::unique_ptr<Collider>  m_collider;
};

struct test
{
	int a = 0;
};

struct test2
{
	int a = 0;
};