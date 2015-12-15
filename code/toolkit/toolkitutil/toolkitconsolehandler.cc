//------------------------------------------------------------------------------
//  toolkitconsolehandler.cc
//  (C) 2015 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "toolkitconsolehandler.h"
#include "threading\thread.h"

namespace ToolkitUtil
{
__ImplementClass(ToolkitUtil::ToolkitConsoleHandler, 'TCCH', IO::ConsoleHandler);
__ImplementInterfaceSingleton(ToolkitUtil::ToolkitConsoleHandler);

using namespace Util;

//------------------------------------------------------------------------------
/**
*/
ToolkitConsoleHandler::ToolkitConsoleHandler() : logLevel(LogError)
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

	this->Append({ LogInfo, str });	
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
	this->Append({ LogError, s });	
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
	this->Append({ LogWarning, s });	
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
	this->Append({ LogDebug, s });	
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
	Threading::ThreadId id = Threading::Thread::GetMyThreadId();
	if (this->log.Contains(id))
	{
		this->currentFlags[id] = 0;
		this->log[id].Clear();
	}	
}

//------------------------------------------------------------------------------
/**
*/
Util::Array<Util::String>
ToolkitConsoleHandler::GetErrors()
{
	Threading::ThreadId id = Threading::Thread::GetMyThreadId();
	Util::Array<Util::String> errors;
	for (Util::Array<LogEntry>::Iterator iter = this->log[id].Begin(); iter != this->log[id].End(); iter++)
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
	Threading::ThreadId id = Threading::Thread::GetMyThreadId();
	for (Util::Array<LogEntry>::Iterator iter = this->log[id].Begin(); iter != this->log[id].End(); iter++)
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
	return this->log[Threading::Thread::GetMyThreadId()];
}

//------------------------------------------------------------------------------
/**
*/
void
ToolkitConsoleHandler::Append(const LogEntry& entry)
{
	this->cs.Enter();
	Threading::ThreadId id = Threading::Thread::GetMyThreadId();
	if (!this->log.Contains(id))
	{
		this->log.Add(id, Util::Array<LogEntry>());
		this->currentFlags.Add(id, entry.level);
	}	
	this->log[id].Append(entry);
	this->currentFlags[id] |= entry.level;
	this->cs.Leave();
}

} // namespace ToolkitUtil
