#pragma once

#include "Frojengine\Object\Object.h"

class Hero : public CObject
{
public:
	virtual void Update(float deltaTime);
	virtual void Render();
};