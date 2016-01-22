//------------------------------------------------------------------------------
//	cmdlineargs.cc
//  2012 Gustav Sterbrant
//------------------------------------------------------------------------------

#include "cmdlineargs.h"
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

namespace AnyFX
{

//------------------------------------------------------------------------------
/**
*/
CmdLineArgs::CmdLineArgs(int argc, char** argv)
{
	int i;
	for (i = 0; i < argc; i++)
	{
		std::string arg(argv[i]);
		this->arguments.push_back(arg);
	}
}

//------------------------------------------------------------------------------
/**
*/
bool
CmdLineArgs::HasArgument(const std::string& key)
{
	return (this->FindIndex(key) != -1);
}

//------------------------------------------------------------------------------
/**
*/
const std::string&
CmdLineArgs::GetArgument(const std::string& key)
{
	int index = this->FindIndex(key);
	assert(index != -1);
	return this->arguments[index+1];
}

//------------------------------------------------------------------------------
/**
*/
std::vector<std::string>
CmdLineArgs::GetArguments(const std::string& key)
{
	std::vector<std::string> args;
	unsigned i;
	for (i = 0; i < this->arguments.size(); i++)
	{
		if (this->arguments[i].substr(0, key.size()) == key)
		{
			args.push_back(this->arguments[i]);
		}
	}
	return args;
}

//------------------------------------------------------------------------------
/**
*/
bool
CmdLineArgs::GetBool(const std::string& key)
{
	int index = this->FindIndex(key);
	assert(index != -1);
	const std::string& value = this->arguments[index+1];
	if (value.compare("true") == 0)
	{
		return true;
	}
	else
	{
		return false;
	}
}

//------------------------------------------------------------------------------
/**
*/
int
CmdLineArgs::GetInt(const std::string& key)
{
	int index = this->FindIndex(key);
	assert(index != -1);
	return atoi(this->arguments[index+1].c_str());
}


//------------------------------------------------------------------------------
/**
*/
int
CmdLineArgs::FindIndex(const std::string& key)
{
	unsigned i;
	for (i = 0; i < this->arguments.size(); i++)
	{
		if (this->arguments[i].compare(key) == 0)
		{
			return i;
		}
	}
	return -1;
}

} // namespace AnyFX