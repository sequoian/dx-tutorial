#pragma once

#include "Entity.h"

class Event
{
protected:
	virtual ~Event() {};
};


class CollisionEvent : public Event
{
public:
	CollisionEvent(Entity a, Entity b) : entityA{ a }, entityB{ b } {}
	Entity entityA;
	Entity entityB;
};