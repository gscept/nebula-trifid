#pragma once
//------------------------------------------------------------------------------
/**
    @class Script::InfoLog
    
    To an info log object you can add several strings which can contain
    different information like debug information or error messages.

    You can also specify a log level, a short description and a source (e.g.
    information on the creator of the info log object).

    Additionally you can open and close sections for a better seperation of
    general and detailed information. Within a section you can open new
    sections, too but there is no check if the amount of openings fits to the
    amount of closings. For further details see Description of BeginSection()
    and EndSection().

    Per default description, source and the list of information strings are
    empty. The log level will be 'Non'.

    An info log object is based on an array of strings.
   
    (C) 2006 Radon Labs GmbH
    (C) 2013-2016 Individual contributors, see AUTHORS file
*/
#include "core/refcounted.h"

//------------------------------------------------------------------------------
namespace Script
{
//------------------------------------------------------------------------------
class InfoLog : public Core::RefCounted
{
    __DeclareClass(InfoLog);

public:

    /// A log level which specifies type or/and origin of the info log object.
    /// It's possible to combine different log levels by using bit operators.
    enum LogLevel
    {
        /// no log level (general information)
        Non         = 0,
        /// debug information
        Debug       = (1<<0),
        /// warning information
        Warning     = (1<<1),
        /// error information
        Error       = (1<<2),

        /// information out of a special domain
        Graphic     = (1<<3),
        Sound       = (1<<4),
        LevelDesign = (1<<5),
        Programming = (1<<6),
        Testing     = (1<<7),
        // ADVICE: don't forget to extend ToString methods in case of adding
        // new log levels
    };

    /// label which is used to mark begin section tag
    static const Util::String BEGINLABEL;
    /// label which is used to mark end section tag
    static const Util::String ENDLABEL;

    /// constructor
    InfoLog();
    /// destructor
    virtual ~InfoLog();

    /// sets the short description
    void SetDescription(Util::String description);
    /// gets the short description
    const Util::String& GetDescription() const;

    /// sets the source of this info log (e.g. creator)
    void SetSource(Util::String source);
    /// gets the source of this info log (e.g. creator)
    const Util::String& GetSource() const;

    /// sets the log level
    void SetLogLevel(LogLevel logLvl);
    /// gets the log level
    LogLevel GetLogLevel() const;

    /// adds an information string to this info log
    void AddInfo(const Util::String& infoString);
    /// adds an array of information strings to this info log
    void AddInfo(const Util::Array<Util::String>& infoArray);
    /// gets the list of information strings of this info log
    const Util::Array<Util::String>& GetInfo() const;

    /// starts new section
    void BeginSection(const Util::String& beginInfo);
    /// ends current section
    void EndSection(const Util::String& endInfo);

    /// returns true if information has been added
    bool HasInfo() const;
    /// returns true if information has been added to current section
    bool HasInfoInCurrentSection() const;

    /// returns a simple string representation of this info log
    Util::String ToString() const;

    /// converts a log level to a string
    static Util::String LogLevelToString(LogLevel logLvl);
    /// converts a string to a log level
    static LogLevel StringToLogLevel(const Util::String& logLevelString);

private:

    /// short desciption
    Util::String description;
    /// source of this info log (e.g. creator)
    Util::String source;
    /// log level
    LogLevel logLvl;

    /// a list of information strings
    Util::Array<Util::String> info;

    /// true if the list of information strings is not empty
    bool hasInfo;

    /// a list of open sections which will be added to the information list when
    /// the first information is added
    Util::Array<Util::String> beginSections;
};
//------------------------------------------------------------------------------
}; // namespace Util
//------------------------------------------------------------------------------