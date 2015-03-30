#pragma once
//------------------------------------------------------------------------------
/**
    @class Audio2::FmodEventCategoryId
      
    Events are grouped into Categorys, defined in the designer, i.e. "GUI", "Athmo" etc.

    (C) 2009 Radon Labs GmbH
    (C) 2013-2014 Individual contributors, see AUTHORS file
*/    
#include "util/string.h"

//------------------------------------------------------------------------------
namespace Audio2
{
class FmodEventCategoryId
{
public:    
    /// constructor
    FmodEventCategoryId();
    /// constructor with arguments
    FmodEventCategoryId(const Util::String &_id);
    /// constructor with char-ptr
    FmodEventCategoryId(const char* ptr);
    /// dtor
    ~FmodEventCategoryId();

    /// assignment from char ptr
    void operator=(const char* ptr);

    /// equality operator
    bool operator==(const FmodEventCategoryId &other) const;
    /// inequality operator
    bool operator!=(const FmodEventCategoryId &other) const;
    /// less than operator
    bool operator<(const FmodEventCategoryId &other) const;
    /// greater than operator
    bool operator>(const FmodEventCategoryId &other) const;

    /// sets the category
    void SetCategory(const Util::String &name);
    /// get s the category
    const Util::String& GetCategory() const;
    /// return true if the FmodEventCategoryId is valid
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
FmodEventCategoryId::FmodEventCategoryId() :
    id()
{
}

//------------------------------------------------------------------------------
/**
*/
inline
FmodEventCategoryId::FmodEventCategoryId(const Util::String &_id) :
    id(_id)
{

}

//------------------------------------------------------------------------------
/**
*/
inline
FmodEventCategoryId::FmodEventCategoryId(const char* ptr) :
    id(ptr)
{
}

//------------------------------------------------------------------------------
/**
*/
inline
FmodEventCategoryId::~FmodEventCategoryId()
{
}

//------------------------------------------------------------------------------
/**
*/
inline
bool 
FmodEventCategoryId::IsValid() const
{
    return this->id.IsValid();
}

//------------------------------------------------------------------------------
/**
*/
inline
void 
FmodEventCategoryId::SetCategory(const Util::String &_id)
{
    this->id = _id;
}

//------------------------------------------------------------------------------
/**
*/
inline
void 
FmodEventCategoryId::Clear()
{
    this->id.Clear();
}

//------------------------------------------------------------------------------
/**
*/
inline
const Util::String&
FmodEventCategoryId::GetCategory() const
{
    return this->id;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool 
FmodEventCategoryId::operator==(const FmodEventCategoryId &other) const
{
    return (this->id == other.id);
}

//------------------------------------------------------------------------------
/**
*/
inline
bool 
FmodEventCategoryId::operator!=(const FmodEventCategoryId &other) const
{
    return (this->id != other.id);
}

//------------------------------------------------------------------------------
/**
*/
inline
bool 
FmodEventCategoryId::operator<(const FmodEventCategoryId &other) const
{
    return (this->id < other.id);
}

//------------------------------------------------------------------------------
/**
*/
inline
bool 
FmodEventCategoryId::operator>(const FmodEventCategoryId &other) const
{
    return (this->id > other.id);
}

//------------------------------------------------------------------------------
/**
*/
inline
void 
FmodEventCategoryId::operator=(const char* ptr)
{
    this->id = ptr;
}


} // namespace Audio2
//------------------------------------------------------------------------------
