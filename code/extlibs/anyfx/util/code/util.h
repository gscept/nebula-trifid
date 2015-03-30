#ifndef UTIL_H
#define UTIL_H
//------------------------------------------------------------------------------
/**
    @file util.h

    Implements Emit, Error and Assert, as well as removes some MSVC warnings.

    2012 Gustav Sterbrant
*/

#include <stdlib.h>
#include <locale.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include <assert.h>
#include <string>

// remove warning for vsnprintf
#pragma warning( disable : 4996 )

// remove incompatibility with C linkage warning
#pragma warning( disable : 4190 )

// remove loss of data conversion from double to float
#pragma warning( disable : 4244 )

namespace AnyFX
{

//------------------------------------------------------------------------------
/**
    Uses va_list to format message
*/
static void
Emit(const char* msg, ...)
{
    va_list args;
    va_start(args, msg);
    vprintf(msg, args);
	va_end(args);
}

//------------------------------------------------------------------------------
/**
    Simple cout for error
*/
static void
Error(const char* msg)
{
	Emit("error: %s\n", msg);
	abort();
}

//------------------------------------------------------------------------------
/**
	Formats const char* to std::string
*/
static std::string
Format(const char* format, ...)
{
	va_list args;
	va_start(args, format);
	char buf[4096];
	vsnprintf(buf, sizeof(buf), format, args);
	std::string retVal(buf);
	va_end(args);
	return retVal;
}


//------------------------------------------------------------------------------
/**
	Compares const char* with std::string
*/
static bool
operator==(const char* lhs, const std::string& rhs)
{
	return rhs.compare(lhs) == 0;
}

//------------------------------------------------------------------------------
/**
	Compares std::string with const char*
*/
static bool
operator==(const std::string& lhs, const char* rhs)
{
	return lhs.compare(rhs) == 0;
}

} // namespace AnyFX
#endif // UTIL_H
