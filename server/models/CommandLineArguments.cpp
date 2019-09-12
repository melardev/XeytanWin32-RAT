#include "CommandLineArguments.h"

#include <iostream>
#include <Windows.h>
#include <shellapi.h>


CommandLineArguments::CommandLineArguments(int argc, char* argv[]) :
	argc_(argc),
	argv_(argv)
{
	appName_ = argv_[0];
	this->parse();
}

CommandLineArguments::CommandLineArguments()
{
	wchar_t* args = GetCommandLineW();
	int argc;
	CommandLineToArgvW(args, &argc);
}

CommandLineArguments::~CommandLineArguments()
{
}

UiMode CommandLineArguments::getUiMode()
{
	return Win32Console;
}

std::string CommandLineArguments::getAppName() const
{
	return appName_;
}

void CommandLineArguments::parse()
{
	typedef std::pair<std::string, std::string> Option;
	Option* option = new std::pair<std::string, std::string>();
	for (const char* const * i = this->begin() + 1; i != this->end(); i++)
	{
		const std::string p = *i;
		if (option->first.empty() && p[0] == '-')
		{
			option->first = p;
			if (i == this->last())
			{
				options_.insert(Option(option->first, option->second));
			}
			continue;
		}

		if (!option->first.empty() && p[0] == '-')
		{
			option->second = "null"; /* or leave empty? */
			options_.insert(Option(option->first, option->second));
			option->first = p;
			option->second = "";
			if (i == this->last())
			{
				options_.insert(Option(option->first, option->second));
			}
			continue;
		}

		if (!option->first.empty())
		{
			option->second = p;
			options_.insert(Option(option->first, option->second));
			option->first = "";
			option->second = "";
			continue;
		}
	}
}

void CommandLineArguments::printOptions() const
{
	std::map<std::string, std::string>::const_iterator m = options_.begin();
	int i = 0;
	if (options_.empty())
	{
		std::cout << "No parameters\n";
	}
	for (; m != options_.end(); m++, ++i)
	{
		std::cout << "Parameter [" << i << "] [" << (*m).first << " " << (*m).second
			<< "]\n";
	}
}

const char* const * CommandLineArguments::begin() const
{
	return argv_;
}

const char* const * CommandLineArguments::end() const
{
	return argv_ + argc_;
}

const char* const * CommandLineArguments::last() const
{
	return argv_ + argc_ - 1;
}

bool CommandLineArguments::hasKey(const std::string& key) const
{
	return options_.find(key) != options_.end();
}

CommandLineArguments::Option* CommandLineArguments::getParamFromKey(
	const std::string& key) const
{
	const Options::const_iterator i = options_.find(key);
	CommandLineArguments::Option* o = 0;
	if (i != options_.end())
	{
		o = new CommandLineArguments::Option((*i).first, (*i).second);
	}
	return o;
}
