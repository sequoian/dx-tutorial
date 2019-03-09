#pragma once

class Resource
{
public:
	virtual bool StartUp()
	{
		return true;
	}

	virtual void ShutDown()
	{

	}
};