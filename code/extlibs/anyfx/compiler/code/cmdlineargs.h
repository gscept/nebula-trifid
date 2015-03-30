#pragma once
//------------------------------------------------------------------------------
/**
    @class CmdLineArgs

	Practical utility class for handling command line arguments

    2012 Gustav Sterbrant
*/
#include <vector>
#include <string>

namespace AnyFX
{

class CmdLineArgs
{
public:
	/// constructor
	CmdLineArgs(int argc, char** argv);

	/// returns true if arguments exist
	bool HasArgument(const std::string& key);
	/// returns argument as string
	const std::string& GetArgument(const std::string& key);
	/// returns arguments as string list
	std::vector<std::string> GetArguments(const std::string& key);
	/// returns argument as bool
	bool GetBool(const std::string& key);
	/// returns argument as integer
	int GetInt(const std::string& key);
private:

	/// finds index of argument
	int FindIndex(const std::string& key);

	std::vector<std::string> arguments;
};



} // namespace AnyFX