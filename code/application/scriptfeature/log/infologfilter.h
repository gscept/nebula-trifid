#pragma once
//------------------------------------------------------------------------------
/**
    @class Script::InfoLogFilter
    
    Provides methods to filter arrays of info log objects by the info log's
    description, source, log level or/and information strings.

    Additionally it provides methods to check if an info log object contains
    a string pattern in it's description, source or information strings or if
    it's log level fits to log level mask.
   
    (C) 2006 Radon Labs GmbH
    (C) 2013-2016 Individual contributors, see AUTHORS file
*/
#include "core/refcounted.h"
#include "scriptfeature/log/infolog.h"

//------------------------------------------------------------------------------
namespace Script
{
//------------------------------------------------------------------------------
class InfoLogFilter : public Core::RefCounted
{
    __DeclareClass(InfoLogFilter);

public:

    /// returns true if description of the info log contains specified string
    /// pattern
    static bool ContainsDescription(
            Ptr<Script::InfoLog> il, const Util::String& dPattern);
    /// returns true if source of the info log contains specified string
    /// pattern
    static bool ContainsSource(
            Ptr<Script::InfoLog> il, const Util::String& sPattern);
    /// returns true if log level of the info log fits to the specified log
    /// level mask
    static bool ContainsLogLevel(
            Ptr<Script::InfoLog> il, Script::InfoLog::LogLevel logLvlMask);
    /// returns true if information of the info log contains specified string
    /// pattern
    static bool ContainsInformation(
            Ptr<Script::InfoLog> il, const Util::String& iPattern);

    /// returns all info logs which contain specified description pattern
    static Util::Array<Ptr<Script::InfoLog> > FilterByDescription(
            const Util::Array<Ptr<Script::InfoLog> >& ils, const Util::String& dPattern);
    /// returns all info logs which contain specified source pattern
    static Util::Array<Ptr<Script::InfoLog> > FilterBySource(
            const Util::Array<Ptr<Script::InfoLog> >& ils, const Util::String& sPattern);
    /// returns all info logs which fit to the specified log level mask
    static Util::Array<Ptr<Script::InfoLog> > FilterByLogLevel(
            const Util::Array<Ptr<Script::InfoLog> >& ils, Script::InfoLog::LogLevel logLvlMask);
    /// returns all info logs which contain specified information pattern
    static Util::Array<Ptr<Script::InfoLog> > FilterByInformation(
            const Util::Array<Ptr<Script::InfoLog> >& ils, const Util::String& iPattern);

    /// returns all info logs which contain specified description, source and
    /// information pattern and fit to the specified log level mask
    static Util::Array<Ptr<Script::InfoLog> > Filter(
            const Util::Array<Ptr<Script::InfoLog> >& ils,
            const Util::String& dPattern,
            const Util::String& sPattern,
            Script::InfoLog::LogLevel logLvlMask,
            const Util::String& iPattern);
};
//------------------------------------------------------------------------------
}; // namespace Util
//------------------------------------------------------------------------------
