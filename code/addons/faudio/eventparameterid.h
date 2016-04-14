#pragma once
//------------------------------------------------------------------------------
/**
    @class FAudio::EventParameterId
      
    Events can have parameters, which is a float value
    
    (C) 2015-2016 Individual contributors, see AUTHORS file
*/    
#include "util/string.h"

//------------------------------------------------------------------------------
namespace FAudio
{
class EventParameterId
{
public:    
    /// constructor
    EventParameterId();
    /// constructor with arguments
    EventParameterId(const Util::String &_id);
    /// constructor with char-ptr
    EventParameterId(const char* ptr);
    /// dtor
    ~EventParameterId();

    /// assignment from char ptr
    void operator=(const char* ptr);

    /// equality operator
    bool operator==(const EventParameterId &other) const;
    /// inequality operator
    bool operator!=(const EventParameterId &other) const;
    /// less than operator
    bool operator<(const EventParameterId &other) const;
    /// greater than operator
    bool operator>(const EventParameterId &other) const;

    /// sets the category
    void SetParameterName(const Util::String &name);
    /// get s the category
    const Util::String& GetParameterName() const;
    /// return true if the EventParameterId is valid
    bool IsValid() const;
    /// clear content
    void Clear();

private:
    Util::String id;
};

//------------------------------------------------------------------------------
/**
*/
inline
EventParameterId::EventParameterId() :
    id()
{
}

//------------------------------------------------------------------------------
/**
*/
inline
EventParameterId::EventParameterId(const Util::String &_id) :
    id(_id)
{

}

//------------------------------------------------------------------------------
/**
*/
inline
EventParameterId::EventParameterId(const char* ptr) :
    id(ptr)
{
}

//------------------------------------------------------------------------------
/**
*/
inline
EventParameterId::~EventParameterId()
{
}

//------------------------------------------------------------------------------
/**
*/
inline
bool 
EventParameterId::IsValid() const
{
    return this->id.IsValid();
}

//------------------------------------------------------------------------------
/**
*/
inline
void 
EventParameterId::SetParameterName(const Util::String &_id)
{
    this->id = _id;
}

//------------------------------------------------------------------------------
/**
*/
inline
void 
EventParameterId::Clear()
{
    this->id.Clear();
}

//------------------------------------------------------------------------------
/**
*/
inline
const Util::String&
EventParameterId::GetParameterName() const
{
    return this->id;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool 
EventParameterId::operator==(const EventParameterId &other) const
{
    return (this->id == other.id);
}

//------------------------------------------------------------------------------
/**
*/
inline
bool 
EventParameterId::operator!=(const EventParameterId &other) const
{
    return (this->id != other.id);
}

//------------------------------------------------------------------------------
/**
*/
inline
bool 
EventParameterId::operator<(const EventParameterId &other) const
{
    return (this->id < other.id);
}

//------------------------------------------------------------------------------
/**
*/
inline
bool 
EventParameterId::operator>(const EventParameterId &other) const
{
    return (this->id > other.id);
}

//------------------------------------------------------------------------------
/**
*/
inline
void 
EventParameterId::operator=(const char* ptr)
{
    this->id = ptr;
}


} // namespace FAudio
//------------------------------------------------------------------------------
