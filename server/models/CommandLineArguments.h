#pragma once
#include "../ui/mediators/IUiMediator.h"
#include <map>

class CommandLineArguments
{
public:
	CommandLineArguments(int argc, char* argv[]);
	CommandLineArguments();
	virtual ~CommandLineArguments();
	UiMode getUiMode();

	typedef std::pair<std::string, std::string> Option;

	std::string getAppName() const;
	bool hasKey(const std::string&) const;
	Option* getParamFromKey(const std::string&) const;
	void printOptions() const;
private:
	typedef std::map<std::string, std::string> Options;
	void parse();
	const char* const * begin() const;
	const char* const * end() const;
	const char* const * last() const;
	Options options_;
	int argc_;
	char** argv_;
	std::string appName_;
};
