#pragma once
class ISignal
{
public:
	ISignal();
	virtual ~ISignal();
	virtual bool wait() = 0;
	virtual bool emit() = 0;
};

