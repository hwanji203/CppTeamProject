#pragma once
#include "Rigidbody.h"
#include "Collider.h"
#include "Pawn.h"
#include <memory>

class Enemy : public Pawn
{
public:
	Enemy(const Vector2* playerPos, int power = 1, Vector2 pos = { 0, 0 });

	virtual void Tick() override;
	virtual void Render() const override;

	virtual void RemovePrevPos() const override;

private:
	int m_power;
	const Vector2* m_playerPos;
	bool m_isSetVelocity;
};