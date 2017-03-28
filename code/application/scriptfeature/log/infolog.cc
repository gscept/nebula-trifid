//------------------------------------------------------------------------------
//  scriptfeature/log/infolog.cc
//  (C) 2006 Radon Labs GmbH
//  (C) 2013-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "scriptfeature/log/infolog.h"

namespace Script
{
__ImplementClass(InfoLog, 'SCIL', Core::RefCounted);

const Util::String InfoLog::BEGINLABEL("$beg$");
const Util::String InfoLog::ENDLABEL("$end$");

//------------------------------------------------------------------------------
/**
    constructor
*/
InfoLog::InfoLog() :
    logLvl(InfoLog::Non),
    hasInfo(false)
{
    // empty
}

//------------------------------------------------------------------------------
/**
    destructor
*/
InfoLog::~InfoLog()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    Sets the description.
*/
void
InfoLog::SetDescription(Util::String description)
{
    this->description = description;
}

//------------------------------------------------------------------------------
/**
    Gets the description.
*/
const Util::String&
InfoLog::GetDescription() const
{
    return this->description;
}

//------------------------------------------------------------------------------
/**
    Sets the source of this info log (e.g. creator).
*/
void
InfoLog::SetSource(Util::String source)
{
    this->source = source;
}

//------------------------------------------------------------------------------
/**
    Gets the source of this info log (e.g. creator).
*/
const Util::String&
InfoLog::GetSource() const
{
    return this->source;
}

//------------------------------------------------------------------------------
/**
    Sets the log level. There will be no check if the log level is supported.
*/
void
InfoLog::SetLogLevel(LogLevel logLvl)
{
    this->logLvl = logLvl;
}

//------------------------------------------------------------------------------
/**
    Gets the log level.
*/
InfoLog::LogLevel
InfoLog::GetLogLevel() const
{
    return this->logLvl;
}

//------------------------------------------------------------------------------
/**
    Adds an information string to this info log.
*/
void
InfoLog::AddInfo(const Util::String& infoString)
{
    this->hasInfo = true;

    // copy begin section statements of list of open sections to list of
    // information strings
    this->info.AppendArray(this->beginSections);
    this->beginSections.Clear();

    this->info.Append(infoString);
}

//------------------------------------------------------------------------------
/**
    Adds an array of information strings to this info log.
*/
void
InfoLog::AddInfo(const Util::Array<Util::String>& infoArray)
{
    // array of information strings is not empty
    if(0 < infoArray.Size())
    {
        this->hasInfo = true;

        // copy begin section statements of list of open sections to list of
        // information strings
        this->info.AppendArray(this->beginSections);
        this->beginSections.Clear();

        this->info.AppendArray(infoArray);
    }
}

//------------------------------------------------------------------------------
/**
    Gets a list of information strings.
*/
const Util::Array<Util::String>&
InfoLog::GetInfo() const
{
    return this->info;
}

//------------------------------------------------------------------------------
/**
    Start new info log section.

    The begin tags for open sections will be added to the list of information
    strings when the first information in this section has been added.

    The begin section tag will have following format:
    "<InfoLog::BEGINLABEL><beginInfo>".

    See EndSection().
    See InfoLog::BEGINLABEL.
*/
void
InfoLog::BeginSection(const Util::String& beginInfo)
{
    // note begin of new section in list open sections
    this->beginSections.Append(InfoLog::BEGINLABEL + beginInfo);
}

//------------------------------------------------------------------------------
/**
    End current info log section.

    If there is a begin section tag without information the begin and the end
    section tags won't be added to the info log object.
    If there is a begin section tag with following information the end section
    tag will be added to the info log object.

    The end section tag will always end the last open section tag, therefor to
    every begin section tag an end section tag should be placed. This method
    does not check if the amount of begin tags fits to the amount of end tags!

    The end section tag will have following format:
    "<InfoLog::ENDLABEL><endInfo>"

    See BeginSection().
    See InfoLog::ENDLABEL.
*/
void
InfoLog::EndSection(const Util::String& endInfo)
{
    int size = this->beginSections.Size();

    if(size > 0)
    {
        // there are open sections without infos
        this->beginSections.EraseIndex(size - 1);
    }
    else
    {
        // there are no open sections without infos
        this->info.Append(InfoLog::ENDLABEL + endInfo);
    }
}

//------------------------------------------------------------------------------
/**
    Returns true if information has been added.
*/
bool
InfoLog::HasInfo() const
{
    return this->hasInfo;
}

//------------------------------------------------------------------------------
/**
    Returns true if information has been added to current section.
*/
bool
InfoLog::HasInfoInCurrentSection() const
{
    // if there are still open begin sections in list no info has been added
    return this->beginSections.Size() == 0;
}

//------------------------------------------------------------------------------
/**
    Returns an simple string representation of the information strings. The
    begin and end labels will be removed and instead the inner sections will be
    indented.
*/
Util::String
InfoLog::ToString() const
{
    // the string representation of the information strings
    Util::String str;

    // build header information
    str += "<InfoLog Description='" + this->description
                + "' Source='" + this->source
                + "' LogLevel='" + InfoLog::LogLevelToString(this->logLvl) + "'>\n";

    // format information strings
    Util::String currentInfo; // current information string
    Util::String indent = "  "; // current indent string
    int i;
    for(i = 0; i < this->info.Size(); i++)
    {
        // get current information string
        currentInfo = this->info[i];

        // info starts with end tag
        if (Util::String::MatchPattern(currentInfo, InfoLog::ENDLABEL + "*"))
        {
            // decrease indent if possible
            if (2 <= indent.Length())
            {
                indent.TerminateAtIndex(indent.Length() - 2);
            }
            // remove end tag
            currentInfo = currentInfo.ExtractToEnd(5);
        }

        // add current information string with indention and line seperator
        str += indent;

        // info starts with begin tag
        if (Util::String::MatchPattern(currentInfo, InfoLog::BEGINLABEL + "*"))
        {
            // increase indent
            indent += "  ";
            // remove begin tag
            currentInfo = currentInfo.ExtractToEnd(5);
        }

         str += currentInfo + "\n";
    }

    // build trailer information
    str += "</InfoLog>\n";

    return str;
}

//------------------------------------------------------------------------------
/**
    Converts a log level to a string.

    If there are set more than one bit of the log level the names of the log
    levels will be seperated by " | ".
    The string will be empty if no set bit of the log level correspond to a
    supported log level.
*/
Util::String
InfoLog::LogLevelToString(InfoLog::LogLevel logLvl)
{
    // no bit is set
    if (logLvl == InfoLog::Non)
        return "Non";
    // some bits are set
    else
    {
        Util::String str = "";

        // add name of log level if bit is set and add " | " as seperator
        if (0 != (logLvl & InfoLog::Debug))
            str += "Debug | ";
        if (0 != (logLvl & InfoLog::Warning))
            str += "Warning | ";
        if (0 != (logLvl & InfoLog::Error))
            str += "Error | ";
        if (0 != (logLvl & InfoLog::Graphic))
            str += "Graphic | ";
        if (0 != (logLvl & InfoLog::Sound))
            str += "Sound | ";
        if (0 != (logLvl & InfoLog::LevelDesign))
            str += "LevelDesign | ";
        if (0 != (logLvl & InfoLog::Programming))
            str += "Programming | ";
        if (0 != (logLvl & InfoLog::Testing))
            str += "Testing | ";

        // remove last separator
        if (3 <= str.Length())
        {
            str.TerminateAtIndex(str.Length() - 3);
        }

        return str;
    }
}

//------------------------------------------------------------------------------
/**
    Converts a string to a log level.

    This method uses a simple pattern matching. If the name of a log level is in
    the log level string the corresponding bit will be set.

    It's not case sensitiv. Not supported log level names will be ignored. If
    there are no supported log levels the method returns log level "Non".
*/
InfoLog::LogLevel
InfoLog::StringToLogLevel(const Util::String& logLevelString)
{
    LogLevel logLvl = InfoLog::Non;

    Util::String str = logLevelString;
    str.ToLower();

    if (Util::String::MatchPattern(str, "*debug*"))
        logLvl = (LogLevel)(logLvl | InfoLog::Debug);
    if (Util::String::MatchPattern(str, "*warning*"))
        logLvl = (LogLevel)(logLvl | InfoLog::Warning);
    if (Util::String::MatchPattern(str, "*error*"))
        logLvl = (LogLevel)(logLvl | InfoLog::Error);
    if (Util::String::MatchPattern(str, "*graphic*"))
        logLvl = (LogLevel)(logLvl | InfoLog::Graphic);
    if (Util::String::MatchPattern(str, "*sound*"))
        logLvl = (LogLevel)(logLvl | InfoLog::Sound);
    if (Util::String::MatchPattern(str, "*leveldesign*"))
        logLvl = (LogLevel)(logLvl | InfoLog::LevelDesign);
    if (Util::String::MatchPattern(str, "*programming*"))
        logLvl = (LogLevel)(logLvl | InfoLog::Programming);
    if (Util::String::MatchPattern(str, "*testing*"))
        logLvl = (LogLevel)(logLvl | InfoLog::Testing);

    return logLvl;
}

}; // namespace util