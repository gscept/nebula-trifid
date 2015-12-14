#pragma once
//------------------------------------------------------------------------------
/**
    @class ToolkitUtil::TookitConsoleHandler

	Console handler for toolkit applications that will not abort in case of errors but 
	log the result of operations so that parent applications can handle the errors
    
    (C) 2015 Individual contributors, see AUTHORS file
*/
#include "io/consolehandler.h"
#include "util/array.h"
#include "util/string.h"
#include "core/singleton.h"

//------------------------------------------------------------------------------
namespace ToolkitUtil
{
class ToolkitConsoleHandler : public IO::ConsoleHandler
{
	__DeclareClass(ToolkitConsoleHandler);
	__DeclareInterfaceSingleton(ToolkitConsoleHandler);
public:
    /// constructor
	ToolkitConsoleHandler();
    /// destructor
	virtual ~ToolkitConsoleHandler();
      
    /// called by console to output data
    virtual void Print(const Util::String& s);
    /// called by console with serious error
    virtual void Error(const Util::String& s);
    /// called by console to output warning
    virtual void Warning(const Util::String& s);
    /// called by console to output debug string
    virtual void DebugOut(const Util::String& s);

	/// write output to console as well, not just store
	void SetConsoleOutputLevel(unsigned char logLevel);

	enum LogEntryLevel
	{
		LogDebug = 0x01,
		LogInfo = 0x02,
		LogWarning = 0x04,
		LogError   = 0x08,
		//
		LogAll	   = 0xff,
	};

	struct LogEntry
	{
		LogEntryLevel level;
		Util::String message;		
	};

	/// clear buffers
	void Clear();
	/// get errors
	Util::Array<Util::String> GetErrors();
	/// get errors
	Util::Array<Util::String> GetWarnings();
	/// get full console output
	const Util::Array<LogEntry> & GetLog();
	/// what kinds of messages occurred since last clear
	unsigned char GetLevels() const;

private:    
	Util::Array<LogEntry> log;	
	unsigned char currentFlags;
	unsigned char logLevel;
};

/**

*/
inline unsigned char
ToolkitConsoleHandler::GetLevels() const
{
	return this->currentFlags;
}

/**

*/
inline void
ToolkitConsoleHandler::SetConsoleOutputLevel(unsigned char level)
{
	this->logLevel = level;
}

} // namespace ToolkitUtil
//------------------------------------------------------------------------------
