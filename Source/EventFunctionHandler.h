#pragma once

#include "Events.h"

class EventFunctionHandlerBase{
public:
	// Call the member function
	void Execute(Event* e)
	{
		Call(e);
	}

private:
	virtual void Call(Event* e) = 0;
};


template<class T, class EventType>
class EventMemberFunctionHandler : public EventFunctionHandlerBase
{
public:
	typedef void (T::*MemberFunction)(EventType*);

	EventMemberFunctionHandler(T* instance, MemberFunction memberFunction) : m_instance{ instance }, m_memberFunction{ memberFunction }
	{
	}

	void Call(Event* e) override
	{
		// Cast event to the correct type and call member function
		(m_instance->*m_memberFunction)(static_cast<EventType*>(e));
	}

private:
	// Pointer to class instance
	T* m_instance;

	// Pointer to member function
	MemberFunction m_memberFunction;
};