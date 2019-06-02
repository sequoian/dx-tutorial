#pragma once

#include <map>
#include <list>
#include <typeindex>
#include "EventFunctionHandler.h"

class EventBus 
{
public:
	~EventBus()
	{
		ShutDown();
	}

	void ShutDown()
	{
		for (auto& subscriber : m_subscribers)
		{
			for (auto& handler : subscriber.second)
			{
				delete handler;
			}
		}
	}

	template<typename EventType>
	void Publish(EventType* e)
	{
		HandlerList* handlers = &m_subscribers[typeid(EventType)];

		for (auto& handler : *handlers)
		{
			handler->Execute(e);
		}
	}

	template<class T, class EventType>
	void Subscribe(T* instance, void (T::*memberFunction)(EventType*))
	{
		HandlerList* handlers = &m_subscribers[typeid(EventType)];
		handlers->push_back(new EventMemberFunctionHandler<T, EventType>(instance, memberFunction));
	}

private:
	typedef std::list<EventFunctionHandlerBase*> HandlerList;
	std::map<std::type_index, HandlerList> m_subscribers;
};