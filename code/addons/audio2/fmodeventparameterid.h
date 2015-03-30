#pragma once
//------------------------------------------------------------------------------
/**
    @class Audio2::FmodEventParameterId
      
    Events can have parameters, which is a float value

    (C) 2009 Radon Labs GmbH
    (C) 2013-2014 Individual contributors, see AUTHORS file
*/    
#include "util/string.h"

//------------------------------------------------------------------------------
namespace Audio2
{
class FmodEventParameterId
{
public:    
    /// constructor
    FmodEventParameterId();
    /// constructor with arguments
    FmodEventParameterId(const Util::String &_id);
    /// constructor with char-ptr
    FmodEventParameterId(const char* ptr);
    /// dtor
    ~FmodEventParameterId();

    /// assignment from char ptr
    void operator=(const char* ptr);

    /// equality operator
    bool operator==(const FmodEventParameterId &other) const;
    /// inequality operator
    bool operator!=(const FmodEventParameterId &other) const;
    /// less than operator
    bool operator<(const FmodEventParameterId &other) const;
    /// greater than operator
    bool operator>(const FmodEventParameterId &other) const;

    /// sets the category
    void SetParameterName(const Util::String &name);
    /// get s the category
    const Util::String& GetParameterName() const;
    /// return true if the FmodEventParameterId is valid
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
FmodEventParameterId::FmodEventParameterId() :
    id()
{
}

//------------------------------------------------------------------------------
/**
*/
inline
FmodEventParameterId::FmodEventParameterId(const Util::String &_id) :
    id(_id)
{

}

//------------------------------------------------------------------------------
/**
*/
inline
FmodEventParameterId::FmodEventParameterId(const char* ptr) :
    id(ptr)
{
}

//------------------------------------------------------------------------------
/**
*/
inline
FmodEventParameterId::~FmodEventParameterId()
{
}

//------------------------------------------------------------------------------
/**
*/
inline
bool 
FmodEventParameterId::IsValid() const
{
    return this->id.IsValid();
}

//------------------------------------------------------------------------------
/**
*/
inline
void 
FmodEventParameterId::SetParameterName(const Util::String &_id)
{
    this->id = _id;
}

//------------------------------------------------------------------------------
/**
*/
inline
void 
FmodEventParameterId::Clear()
{
    this->id.Clear();
}

//------------------------------------------------------------------------------
/**
*/
inline
const Util::String&
FmodEventParameterId::GetParameterName() const
{
    return this->id;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool 
FmodEventParameterId::operator==(const FmodEventParameterId &other) const
{
    return (this->id == other.id);
}

//------------------------------------------------------------------------------
/**
*/
inline
bool 
FmodEventParameterId::operator!=(const FmodEventParameterId &other) const
{
    return (this->id != other.id);
}

//------------------------------------------------------------------------------
/**
*/
inline
bool 
FmodEventParameterId::operator<(const FmodEventParameterId &other) const
{
    return (this->id < other.id);
}

//------------------------------------------------------------------------------
/**
*/
inline
bool 
FmodEventParameterId::operator>(const FmodEventParameterId &other) const
{
    return (this->id > other.id);
}

//------------------------------------------------------------------------------
/**
*/
inline
void 
FmodEventParameterId::operator=(const char* ptr)
{
    this->id = ptr;
}


} // namespace Audio2
//------------------------------------------------------------------------------
