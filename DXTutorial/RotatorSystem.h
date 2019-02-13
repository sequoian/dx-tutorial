#pragma once

#include "ComponentSystem.h"
#include "Types.h"


struct RotatorComponent
{
	U64 transform;
	float angle;
	float speed;
};