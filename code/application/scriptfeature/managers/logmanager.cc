//------------------------------------------------------------------------------
//  scriptfeature/managers/logmanager.cc
//
//  (C) 2008 Radon Labs GmbH
//  (C) 2013-2015 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "scriptfeature/managers/logmanager.h"

namespace Script
{
__ImplementClass(LogManager, 'SCLM', Game::Manager);
__ImplementSingleton(LogManager);

//------------------------------------------------------------------------------
/**
    constructor
*/
LogManager::LogManager() :
    logEntries(512),
    amountOfLogEntries(0),
    amountOfNewLogEntries(0)
{
    __ConstructSingleton;
}

//------------------------------------------------------------------------------
/**
    destructor
*/
LogManager::~LogManager()
{
    __DestructSingleton;
}

//------------------------------------------------------------------------------
/**
    Called when attached to game server.
*/
void
LogManager::OnActivate()
{
    Manager::OnActivate();

    // initialize ring buffer for log entries
    this->ClearLogEntries();
}

//------------------------------------------------------------------------------
/**
    Called when removed from game server.
*/
void
LogManager::OnDeactivate()
{
    // release resources of log user interface if available
    //if (this->logUI != 0)
    //{
    //    if (this->logUI->IsOpen())
    //    {
    //        this->logUI->Close();
    //    }
    //    this->logUI = 0;
    //}
    Manager::OnDeactivate();
}

////------------------------------------------------------------------------------
///**
//    Gets the log user interface.
//*/
//Ptr<UI::LogUI>
//LogManager::GetLogUI() const
//{
//    return this->logUI;
//}
//
////------------------------------------------------------------------------------
///**
//    Sets the log user interface.
//*/
//void
//LogManager::SetLogUI(Ptr<UI::LogUI> logUI)
//{
//    this->logUI = logUI;
//}


////------------------------------------------------------------------------------
///**
//    Opens the log user interface.
//*/
//void
//LogManager::OpenLogUI()
//{
//    n_assert2(this->logUI != 0, "No LogUI has been set!");
//
//    if (!this->logUI->IsOpen())
//    {
//        this->logUI->Open();
//    }
//}
//
////------------------------------------------------------------------------------
///**
//    Closes the log user interface.
//*/
//void
//LogManager::CloseLogUI()
//{
//    n_assert2(this->logUI != 0, "No LogUI has been set!");
//
//    if (this->logUI->IsOpen())
//    {
//        this->logUI->Close();
//    }
//}

//------------------------------------------------------------------------------
/**
    Puts a log entry with description, source, information and a log level.
*/
void
LogManager::PutLogEntry(
        const Util::String& description,
        const Util::String& source,
        Script::InfoLog::LogLevel logLvl,
        const Util::String& information)
{
    // create new info log object with parameters
    Ptr<Script::InfoLog> newInfoLog = Script::InfoLog::Create();
    newInfoLog->SetDescription(description);
    newInfoLog->SetSource(source);
    newInfoLog->SetLogLevel(logLvl);
    newInfoLog->AddInfo(information);

    // delegate to PutLogEntry(const Ptr<Script::InfoLog>& infoLog)
    this->PutLogEntry(newInfoLog);
}

//------------------------------------------------------------------------------
/**
    Puts a log entry in form of an info log object.
*/
void
LogManager::PutLogEntry(const Ptr<Script::InfoLog>& infoLog)
{
    n_assert(infoLog != 0);

    // if ring buffer is full delete oldest entry
    if (this->logEntries.Size() == this->logEntries.Capacity())
    {
        this->amountOfLogEntries--;
    }

    // add new log entry to ring buffer
    this->logEntries.Add(infoLog);
    Util::String str = infoLog->ToString();

    // print info log to console an log file
    this->Print(infoLog);

    this->amountOfLogEntries++;
    this->amountOfNewLogEntries++;

    //// update log user interface
    //if (this->logUI != 0 && this->logUI->IsOpen())
    //{
    //    this->logUI->Update();
    //}
}

//------------------------------------------------------------------------------
/**
    Gets all log entries.
*/
Util::Array<Ptr<Script::InfoLog> >
LogManager::GetLogEntries()
{
    Util::Array<Ptr<Script::InfoLog> > entries;
    IndexT i;
    for (i = 0; i < this->logEntries.Size(); i++)
    {
        entries.Append(this->logEntries[i]);
    }
    this->amountOfNewLogEntries = 0;
    return entries;
}

//------------------------------------------------------------------------------
/**
    Gets new log entries which have been added since the last getting of log
    entries.
*/
Util::Array<Ptr<Script::InfoLog> >
LogManager::GetNewLogEntries()
{
    // all log entries are new
    if (this->amountOfNewLogEntries >= this->amountOfLogEntries)
    {
        return this->GetLogEntries();
    }
    else
    {
        Util::Array<Ptr<Script::InfoLog> > newEntries;
        IndexT i;
        for (i = this->amountOfNewLogEntries - 1; i >= 0; i--)
        {
            newEntries.Append(this->logEntries[i]);
        }
        this->amountOfNewLogEntries = 0;
        return newEntries;
    }
}

//------------------------------------------------------------------------------
/**
    Deletes all log entries.
*/
void
LogManager::ClearLogEntries()
{
    this->logEntries.Reset();
    this->amountOfLogEntries = 0;
    this->amountOfNewLogEntries = 0;
    //if (this->logUI != 0)
    //{
    //    this->logUI->Update();
    //}
}

//------------------------------------------------------------------------------
/**
    Sets the capacity of the managed ring buffer (and deletes all log entries!).
*/
void
LogManager::SetCapacity(int capacity)
{
    this->logEntries.SetCapacity(capacity);
    this->ClearLogEntries();
}

//------------------------------------------------------------------------------
/**
    Gets the capacity of the managed ring buffer.
*/
int
LogManager::GetCapacity() const
{
    return this->logEntries.Capacity();
}

//------------------------------------------------------------------------------
/**
    Gets the amount of log entries currently managed in the ring buffer.
*/
int
LogManager::GetAmountOfLogEntries() const
{
    return this->amountOfLogEntries;
}

//------------------------------------------------------------------------------
/**
    Gets the amount of log entries which have been added since the last getting
    of log entries.

    This amount can be larger than the capacity of the ring buffer. In this case
    all currently managed log entries are new and the old ones are no longer in
    the ring buffer.
*/
int
LogManager::GetAmountOfNewLogEntries() const
{
    return this->amountOfNewLogEntries;
}

//------------------------------------------------------------------------------
/**
    Prints log entry to console an log file.
*/
void
LogManager::Print(const Ptr<Script::InfoLog>& infoLog)
{
    //print header information
    n_printf("<LogEntry Description='%s' Source='%s' LogLevel='%s'>\n",
				infoLog->GetDescription().AsCharPtr(),
                infoLog->GetSource().AsCharPtr(),
                Script::InfoLog::LogLevelToString(infoLog->GetLogLevel()).AsCharPtr());

    // print information strings with indention

	Util::Array<Util::String> info = infoLog->GetInfo();

    Util::String indent = "    "; // indent string

    Util::String currentInfo; // current information string
    Util::String currentIndent; // curretn indent string
    int currentIndentLvl = 1; // current indent level

    int idx;
    for(idx = 0; idx < info.Size(); idx++)
    {
        currentIndent = "";

        // get current information string
        currentInfo = info[idx];

        // info string starts with end tag
        if (Util::String::MatchPattern(currentInfo, Script::InfoLog::ENDLABEL + "*"))
        {
            // decrease indent
            currentIndentLvl--;
            // remove end tag
            currentInfo = currentInfo.ExtractToEnd(5);
        }

        // build current indent string by current indent level
        int i;
        for (i = 0; i < currentIndentLvl; i++)
        {
            currentIndent += indent;
        }

        // info string starts with begin tag
        if (Util::String::MatchPattern(currentInfo, Script::InfoLog::BEGINLABEL + "*"))
        {
            // increase indent
            currentIndentLvl++;
            // remove begin tag
            currentInfo = currentInfo.ExtractToEnd(5);
        }

        n_printf((currentIndent + currentInfo + "\n").AsCharPtr());
    }

    // print trailer information
    n_printf("</LogEntry>\n");
}

}; // namespace Game