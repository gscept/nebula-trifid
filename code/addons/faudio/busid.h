#pragma once
//------------------------------------------------------------------------------
/**
    @class FAudio::BusId
      
    Events are grouped into Buses, defined in the designer
    
    (C) 2015 Individual contributors, see AUTHORS file
*/    
#include "util/string.h"

//------------------------------------------------------------------------------
namespace FAudio
{
class BusId
{
public:    
    /// constructor
    BusId();
    /// constructor with arguments
    BusId(const Util::String &_id);
    /// constructor with char-ptr
    BusId(const char* ptr);
    /// dtor
    ~BusId();

    /// assignment from char ptr
    void operator=(const char* ptr);

    /// equality operator
	bool operator==(const BusId &other) const;
    /// inequality operator
	bool operator!=(const BusId &other) const;
    /// less than operator
	bool operator<(const BusId &other) const;
    /// greater than operator
	bool operator>(const BusId &other) const;

    /// sets the bus identifier name
    void SetBus(const Util::String &name);
    /// get s the bus identifier name
    const Util::String& GetBus() const;
    /// return true if the BusId is valid
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
BusId::BusId() :
    id()
{
}

//------------------------------------------------------------------------------
/**
*/
inline
BusId::BusId(const Util::String &_id) :
    id(_id)
{

}

//------------------------------------------------------------------------------
/**
*/
inline
BusId::BusId(const char* ptr) :
    id(ptr)
{
}

//------------------------------------------------------------------------------
/**
*/
inline
BusId::~BusId()
{
}

//------------------------------------------------------------------------------
/**
*/
inline
bool 
BusId::IsValid() const
{
    return this->id.IsValid();
}

//------------------------------------------------------------------------------
/**
*/
inline
void 
BusId::SetBus(const Util::String &_id)
{
    this->id = _id;
}

//------------------------------------------------------------------------------
/**
*/
inline
void 
BusId::Clear()
{
    this->id.Clear();
}

//------------------------------------------------------------------------------
/**
*/
inline
const Util::String&
BusId::GetBus() const
{
    return this->id;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool 
BusId::operator==(const BusId &other) const
{
    return (this->id == other.id);
}

//------------------------------------------------------------------------------
/**
*/
inline
bool 
BusId::operator!=(const BusId &other) const
{
    return (this->id != other.id);
}

//------------------------------------------------------------------------------
/**
*/
inline
bool 
BusId::operator<(const BusId &other) const
{
    return (this->id < other.id);
}

//------------------------------------------------------------------------------
/**
*/
inline
bool 
BusId::operator>(const BusId &other) const
{
    return (this->id > other.id);
}

//------------------------------------------------------------------------------
/**
*/
inline
void 
BusId::operator=(const char* ptr)
{
    this->id = ptr;
}


} // namespace FAudio
//------------------------------------------------------------------------------
