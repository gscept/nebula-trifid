//------------------------------------------------------------------------------
//  logger.cc
//  (C) 2008 Radon Labs GmbH
//  (C) 2013-2015 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "toolkitutil/logger.h"

namespace ToolkitUtil
{
using namespace Util;

//------------------------------------------------------------------------------
/**
*/
Logger::Logger() :
    verbose(true)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
Logger::~Logger()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
Logger::Error(const char* msg, ...)
{
    va_list argList;
    va_start(argList, msg);
    String str;
    str.FormatArgList(msg, argList);
	this->messages.Append(String("<font size=\"4\" color=\"red\"><b>[ERROR] ") + str + String("</b></font>"));
    if (this->verbose)
    {
        n_printf(this->messages.Back().AsCharPtr());
    }
    va_end(argList);
}
     
//------------------------------------------------------------------------------
/**
*/
void
Logger::Warning(const char* msg, ...)
{
    va_list argList;
    va_start(argList, msg);
    String str;
    str.FormatArgList(msg, argList);
    this->messages.Append(String("[WARNING] ") + str);
    if (this->verbose)
    {
        n_printf(this->messages.Back().AsCharPtr());
    }
    va_end(argList);
}

//------------------------------------------------------------------------------
/**
*/
void
Logger::Print(const char* msg, ...)
{
    va_list argList;
    va_start(argList, msg);
    String str;
    str.FormatArgList(msg, argList);
    this->messages.Append(str);
    if (this->verbose)
    {
        n_printf(str.AsCharPtr());
    }
    va_end(argList);
}

} // namespace ToolkitUtil
