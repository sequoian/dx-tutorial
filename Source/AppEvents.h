#pragma once

#include "Event.h"

class OpenDoorEvent : public Event
{
public:
	OpenDoorEvent(Entity e) : door{ e } {}
	Entity door;
};