#pragma once
//------------------------------------------------------------------------------
/**
    @class FAudio::EventId

    event path 
    
    (C) 2014-2015 Individual contributors, see AUTHORS file
*/
#include "faudio/busid.h"

//------------------------------------------------------------------------------
namespace FAudio
{
class EventId
{
public:
    /// constructor
    EventId();
    /// constructor with arguments
	EventId(const Util::String &_eventPath);
	/// constructor with arguments
	EventId(const char *_eventPath);
    /// dtor
	~EventId();

    /// sets the EventId, if your
    void Set(const Util::String &_eventPath);
    /// return true if the EventId is valid
    bool IsValid() const;
    /// clear content
    void Clear();

    /// get path in eventproject to event
    const Util::String& GetEventPath() const;

private:
    Util::String eventPath;
};

//------------------------------------------------------------------------------
/**
*/
inline
EventId::EventId() :    
    eventPath()
{
}

//------------------------------------------------------------------------------
/**
*/
inline
EventId::EventId(const Util::String &_eventPath) :    
    eventPath(_eventPath)
{

}

//------------------------------------------------------------------------------
/**
*/
inline
EventId::EventId(const char *_eventPath) :
eventPath(_eventPath)
{

}

//------------------------------------------------------------------------------
/**
*/
inline
EventId::~EventId()
{
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
EventId::IsValid() const
{
    return (this->eventPath.IsValid());
}

//------------------------------------------------------------------------------
/**
*/
inline
void
EventId::Set(const Util::String &_eventPath)
{    
    this->eventPath = _eventPath;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
EventId::Clear()
{    
    this->eventPath.Clear();
}

//------------------------------------------------------------------------------
/**
*/
inline
const Util::String&
EventId::GetEventPath() const
{
    return this->eventPath;
}


} // namespace FAudio
//------------------------------------------------------------------------------
