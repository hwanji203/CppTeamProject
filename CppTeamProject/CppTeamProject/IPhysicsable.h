#pragma once
#include "Enums.h"
class IPhysicsable
{
public:
	virtual ~IPhysicsable() = default;
	virtual void SetVelocity(float velocityX) abstract;
};

