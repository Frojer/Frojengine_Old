#pragma once

#include "Frojengine\Frojengine.h"

class Hero : public CObject
{
public:
	virtual void Update(float deltaTime);
	virtual void Render();
};