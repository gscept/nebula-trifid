//------------------------------------------------------------------------------
//  toolkitconsolehandler.cc
//  (C) 2015 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "toolkitconsolehandler.h"

namespace ToolkitUtil
{
__ImplementClass(ToolkitUtil::ToolkitConsoleHandler, 'TCCH', IO::ConsoleHandler);
__ImplementInterfaceSingleton(ToolkitUtil::ToolkitConsoleHandler);

using namespace Util;

//------------------------------------------------------------------------------
/**
*/
ToolkitConsoleHandler::ToolkitConsoleHandler() : currentFlags(0), logLevel(LogError)
{
	__ConstructInterfaceSingleton;
}

//------------------------------------------------------------------------------
/**
*/
ToolkitConsoleHandler::~ToolkitConsoleHandler()
{
	__DestructInterfaceSingleton;
}

//------------------------------------------------------------------------------
/**
*/
void
ToolkitConsoleHandler::Print(const String& str)
{
	this->log.Append({ LogInfo, str });
	this->currentFlags |= LogInfo;
	if (this->logLevel & LogInfo)
	{
		Core::SysFunc::DebugOut(str.AsCharPtr());
	}
}

//------------------------------------------------------------------------------
/**
*/
void
ToolkitConsoleHandler::Error(const String& s)
{
	this->log.Append({ LogError, s });
	this->currentFlags |= LogError;
	if (this->logLevel & LogError)
	{
		Core::SysFunc::DebugOut(s.AsCharPtr());
	}
}

//------------------------------------------------------------------------------
/**
*/
void
ToolkitConsoleHandler::Warning(const String& s)
{
	this->log.Append({ LogWarning, s });
	this->currentFlags |= LogWarning;
	if (this->logLevel & LogWarning)
	{
		Core::SysFunc::DebugOut(s.AsCharPtr());
	}
}

//------------------------------------------------------------------------------
/**
*/
void
ToolkitConsoleHandler::DebugOut(const String& s)
{
	this->log.Append({ LogDebug, s });
	this->currentFlags |= LogDebug;
	if (this->logLevel & LogDebug)
	{
		Core::SysFunc::DebugOut(s.AsCharPtr());
	}
}

//------------------------------------------------------------------------------
/**
*/
void
ToolkitConsoleHandler::Clear()
{
	this->currentFlags = 0;
	this->log.Clear();
}

//------------------------------------------------------------------------------
/**
*/
Util::Array<Util::String>
ToolkitConsoleHandler::GetErrors()
{
	Util::Array<Util::String> errors;
	for (Util::Array<LogEntry>::Iterator iter = this->log.Begin(); iter != this->log.End(); iter++)
	{
		if (iter->level == LogError)
		{
			errors.Append(iter->message);
		}
	}
	return errors;
}

//------------------------------------------------------------------------------
/**
*/
Util::Array<Util::String>
ToolkitConsoleHandler::GetWarnings()
{
	Util::Array<Util::String> warnings;
	for (Util::Array<LogEntry>::Iterator iter = this->log.Begin(); iter != this->log.End(); iter++)
	{
		if (iter->level == LogWarning)
		{
			warnings.Append(iter->message);
		}
	}
	return warnings;
}
//------------------------------------------------------------------------------
/**
*/
const Util::Array<ToolkitConsoleHandler::LogEntry> &
ToolkitConsoleHandler::GetLog()
{
	return this->log;
}

} // namespace ToolkitUtil
