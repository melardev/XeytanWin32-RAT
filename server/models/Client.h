#pragma once
#include <Rpc.h>
#include <string>

class Client
{
public:

	Client()
	{
	}

	Client(UUID* uuid, SOCKET socketDescriptor);
	Client(UUID* uuid, const char* username, char* pcName);
	~Client();
	std::string getUsername();
	std::string getPcName();
	UUID* getUuid();
	SOCKET getSocketDescriptor();
	void setUsername(const std::string& username);
	void setPcName(const std::string& pcName);
	SOCKET getId() const;

private:
	std::string pcName;
	std::string username;
	UUID* uuid;
	SOCKET socketDescriptor;
};
