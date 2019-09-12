#include "Client.h"
#include <ostream>
#include <iostream>


Client::Client(UUID* uuid, SOCKET socketDescriptor)
	: uuid(uuid), socketDescriptor(socketDescriptor)
{
}

Client::Client(UUID* uuid, const char* username, char* pcName)
	: username(username), pcName(pcName)
{
}

Client::~Client()
{
	std::cout << "Client destructor called" << std::endl;
}

std::string Client::getUsername()
{
	return username;
}

std::string Client::getPcName()
{
	return pcName;
}

UUID* Client::getUuid()
{
	return uuid;
}

SOCKET Client::getSocketDescriptor()
{
	return socketDescriptor;
}

void Client::setUsername(const std::string& username)
{
	this->username = username;
}

void Client::setPcName(const std::string& pcName)
{
	this->pcName = pcName;
}

unsigned long long Client::getId() const
{
	return socketDescriptor;
}
