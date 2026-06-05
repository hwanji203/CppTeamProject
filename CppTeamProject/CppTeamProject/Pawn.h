#pragma once
#include "Actor.h"
#include "IMovable.h"
#include "Rigidbody.h"
#include "Collider.h"
#include <memory>

class Pawn : public Actor, public IMovable
{
public:
	Pawn(Position _pos = { 0,0 });
	~Pawn();

	void Move(Dir _dir) override;

	void Tick() override;
	void Render() const override;

	Rigidbody* GetRigidbody() const { return m_rigidbody.get(); }
	Collider*  GetCollider()  const { return m_collider.get(); }

private:
	std::unique_ptr<Rigidbody> m_rigidbody;
	std::unique_ptr<Collider>  m_collider;
};

