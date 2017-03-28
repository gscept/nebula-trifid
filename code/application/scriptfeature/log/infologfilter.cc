//------------------------------------------------------------------------------
//  scriptfeature/log/infologfilter.cc
//  (C) 2006 Radon Labs GmbH
//  (C) 2013-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "scriptfeature/log/infologfilter.h"

namespace Script
{
__ImplementClass(InfoLogFilter, 'SILF', Core::RefCounted);

//------------------------------------------------------------------------------
/**
    Returns true if description of the info log contains specified string
    pattern.
*/
bool
InfoLogFilter::ContainsDescription(Ptr<Script::InfoLog> il,
                                   const Util::String& dPattern)
{
    n_assert(il != 0);

    if (dPattern.IsEmpty())
    {
        return true;
    }

    Util::String pattern = "*" + dPattern + "*";
    pattern.ToLower();

    Util::String str = il->GetDescription();
    str.ToLower();

    return Util::String::MatchPattern(str, pattern);
}

//------------------------------------------------------------------------------
/**
    Returns true if source of the info log contains specified string pattern.
*/
bool
InfoLogFilter::ContainsSource(Ptr<Script::InfoLog> il,
                              const Util::String& sPattern)
{
    n_assert(il != 0);

    if (sPattern.IsEmpty())
    {
        return true;
    }

    Util::String pattern = "*" + sPattern + "*";
    pattern.ToLower();

    Util::String str = il->GetSource();
    str.ToLower();

    return Util::String::MatchPattern(str, pattern);
}

//------------------------------------------------------------------------------
/**
    Returns true if log level of the info log fits to the specified log level
    mask.
*/
bool
InfoLogFilter::ContainsLogLevel(Ptr<Script::InfoLog> il,
                                Script::InfoLog::LogLevel logLvlMask)
{
    n_assert(il != 0);

    if (Script::InfoLog::Non == logLvlMask)
    {
        return true;
    }

    return 0 != (logLvlMask & il->GetLogLevel());
}

//------------------------------------------------------------------------------
/**
    Returns true if information of the info log contains specified string
    pattern.
*/
bool
InfoLogFilter::ContainsInformation(Ptr<Script::InfoLog> il,
                                   const Util::String& iPattern)
{
    n_assert(il != 0);

    if (iPattern.IsEmpty())
    {
        return true;
    }

    Util::String pattern = "*" + iPattern + "*";
    pattern.ToLower();

    Util::Array<Util::String> info = il->GetInfo();
    int i;
    for (i = 0; i < info.Size(); i++)
    {
        info[i].ToLower();
        if (Util::String::MatchPattern(info[i], pattern))
        {
            return true;
        }
    }

    return false;
}

//------------------------------------------------------------------------------
/**
    Returns all info logs which contain specified description pattern.
*/
Util::Array<Ptr<Script::InfoLog> >
InfoLogFilter::FilterByDescription(const Util::Array<Ptr<Script::InfoLog> >& ils,
                                   const Util::String& dPattern)
{
    return InfoLogFilter::Filter(ils, dPattern, "", Script::InfoLog::Non, "");
}

//------------------------------------------------------------------------------
/**
    Returns all info logs which contain specified source pattern.
*/
Util::Array<Ptr<Script::InfoLog> >
InfoLogFilter::FilterBySource(const Util::Array<Ptr<Script::InfoLog> >& ils,
                              const Util::String& sPattern)
{
    return InfoLogFilter::Filter(ils, "", sPattern, Script::InfoLog::Non, "");
}

//------------------------------------------------------------------------------
/**
    Returns all info logs which fit to the specified log level mask.
*/
Util::Array<Ptr<Script::InfoLog> >
InfoLogFilter::FilterByLogLevel(const Util::Array<Ptr<Script::InfoLog> >& ils,
                                   Script::InfoLog::LogLevel logLvlMask)
{
    return InfoLogFilter::Filter(ils, "", "", logLvlMask, "");
}

//------------------------------------------------------------------------------
/**
    Returns all info logs which contain specified information pattern.
*/
Util::Array<Ptr<Script::InfoLog> >
InfoLogFilter::FilterByInformation(const Util::Array<Ptr<Script::InfoLog> >& ils,
                                   const Util::String& iPattern)
{
    return InfoLogFilter::Filter(ils, "", "", Script::InfoLog::Non, iPattern);
}

//------------------------------------------------------------------------------
/**
    Returns all info logs which contain specified description, source and
    information pattern and fit to the specified log level mask.
*/
Util::Array<Ptr<Script::InfoLog> >
InfoLogFilter::Filter(const Util::Array<Ptr<Script::InfoLog> >& ils,
                      const Util::String& dPattern,
                      const Util::String& sPattern,
                      Script::InfoLog::LogLevel logLvlMask,
                      const Util::String& iPattern)
{
    Util::Array<Ptr<Script::InfoLog> > filtrate;

    int i;
    for (i = 0; i < ils.Size(); i++)
    {
        if (InfoLogFilter::ContainsDescription(ils[i], dPattern)
            && InfoLogFilter::ContainsSource(ils[i], sPattern)
            && InfoLogFilter::ContainsLogLevel(ils[i], logLvlMask)
            && InfoLogFilter::ContainsInformation(ils[i], iPattern))
        {
            filtrate.Append(ils[i]);
        }
    }

    return filtrate;
}

}; // namespace util