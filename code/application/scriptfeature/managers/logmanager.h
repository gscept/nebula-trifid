#pragma once
//------------------------------------------------------------------------------
/**
    @class Managers::LogManager

    The log manager manages log entries in a ring buffer.
    
    You can add log entries (e.g. in form of a info log object) and you can get
    all log entries currently in the ring buffer or new log entries which have
    been added since the last getting of log entries.
    
    A log manager uses a log user interface which will be updated when new log
    entries are added.

    The initial capacity of the managed ring buffer will be 4000 log entries.
    
    (C) 2006 Radon Labs GmbH
    (C) 2013-2016 Individual contributors, see AUTHORS file
*/
#include "core/singleton.h"
#include "game/manager.h"
#include "scriptfeature/log/infolog.h"
#include "util/ringbuffer.h"

//namespace UI
//{
//    class LogUI;
//}

//------------------------------------------------------------------------------
namespace Script
{
class LogManager : public Game::Manager
{
    __DeclareClass(LogManager);
    __DeclareSingleton(LogManager);
public:

    /// constructor
    LogManager();
    /// destructor
    virtual ~LogManager();

    /// called when attached to game server
    virtual void OnActivate();
    /// called when removed from game server
    virtual void OnDeactivate();

    ///// gets the log user interface
    //Ptr<UI::LogUI> GetLogUI() const;
    ///// sets the log user interface
    //virtual void SetLogUI(Ptr<UI::LogUI> logUI);

    ///// opens log user interface
    //virtual void OpenLogUI();
    ///// closes log user interface
    //virtual void CloseLogUI();

    /// puts a log entry with description, source, information and a log level
    virtual void PutLogEntry(
            const Util::String& description,
            const Util::String& source,
            Script::InfoLog::LogLevel,
            const Util::String& information);
    /// puts a log entry in form of an info log object
    virtual void PutLogEntry(const Ptr<Script::InfoLog>& infoLog);

    /// gets all log entries
    virtual Util::Array<Ptr<Script::InfoLog> > GetLogEntries();
    /// gets new log entries which have been added since the last getting of log entries
    virtual Util::Array<Ptr<Script::InfoLog> > GetNewLogEntries();

    /// deletes all log entries
    virtual void ClearLogEntries();

    /// sets the capacity of the managed ring buffer (and deletes all log entries!)
    void SetCapacity(int capacity);
    /// gets the capacity of the managed ring buffer
    int GetCapacity() const;
    /// gets the amount of all log entries currently managed in the ring buffer
    int GetAmountOfLogEntries() const;
    /// gets the amount of log entries which have been added since the last getting of log entries
    int GetAmountOfNewLogEntries() const;

protected:
	Util::RingBuffer<Ptr<Script::InfoLog> > logEntries;
    int amountOfLogEntries;
    int amountOfNewLogEntries;

    /// prints log entry to console an log file
    static void Print(const Ptr<Script::InfoLog>& infoLog);

private:
    ///// log user interface
    //Ptr<UI::LogUI> logUI;
};

}; // namespace Managers
//------------------------------------------------------------------------------