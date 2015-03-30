//------------------------------------------------------------------------------
//  mayalogger.cc
//  (C) 2009 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "mayalogger.h"
#include "io/console.h"
#include <maya/MGlobal.h>

namespace Maya
{
__ImplementClass(Maya::MayaLogger, 'MLOG', Core::RefCounted);
__ImplementSingleton(Maya::MayaLogger);

using namespace Util;
using namespace IO;

//------------------------------------------------------------------------------
/**
*/
MayaLogger::MayaLogger() :
    isValid(false)
{
    __ConstructSingleton;
}    

//------------------------------------------------------------------------------
/**
*/
MayaLogger::~MayaLogger()
{
    if (this->IsValid())
    {
        this->Discard();
    }
    __DestructSingleton;
}

//------------------------------------------------------------------------------
/**
*/
void
MayaLogger::Setup()
{
    n_assert(!this->IsValid());
    this->isValid = true;
}

//------------------------------------------------------------------------------
/**
*/
void
MayaLogger::Discard()
{
    n_assert(this->IsValid());
    this->isValid = false;
}

//------------------------------------------------------------------------------
/**
*/
void
MayaLogger::Print(const char* fmt, ...)
{
    n_assert(this->IsValid());
    va_list argList;
    va_start(argList, fmt);
    String str;
    str.FormatArgList(fmt, argList);
    MGlobal::displayInfo(MString(str.AsCharPtr()));
    Console::Instance()->Print(fmt, argList);
    va_end(argList);
}

//------------------------------------------------------------------------------
/**
*/
void
MayaLogger::Warning(const char* fmt, ...)
{
    n_assert(this->IsValid());
    va_list argList;
    va_start(argList, fmt);
    String str;
    str.FormatArgList(fmt, argList);
    MGlobal::displayWarning(MString(str.AsCharPtr()));
    Console::Instance()->Print(fmt, argList);
    va_end(argList);
}

//------------------------------------------------------------------------------
/**
*/
void
MayaLogger::Error(const char* fmt, ...)
{
    n_assert(this->IsValid());
    va_list argList;
    va_start(argList, fmt);
    String str;
    str.FormatArgList(fmt, argList);
    MGlobal::displayError(MString(str.AsCharPtr()));
    Console::Instance()->Print(fmt, argList);
    va_end(argList);
}

} // namespace Maya