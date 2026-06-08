#pragma once
#include "Rigidbody.h"
#include "Collider.h"
#include "Pawn.h"
#include <memory>

class Enemy : public Pawn
{
public:
	Enemy(Vector2 pos = { 0, 0 }, int power = 1);

	virtual void Tick() override;
	virtual void Render() const override;

private:
	int m_power;
};