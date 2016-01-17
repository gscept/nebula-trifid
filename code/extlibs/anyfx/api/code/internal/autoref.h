#pragma once
//------------------------------------------------------------------------------
/**
    @class Internal::AutoRef
    
    Holds a reference counter. Very simple with a release/retain syntax.
    Release will decrease the refcount, and if it hits 0, deletes the object.
    
    (C) 2014 Gustav Sterbrant
*/
//------------------------------------------------------------------------------
#include <assert.h>

namespace AnyFX
{
class AutoRef
{
public:
	/// constructor
	AutoRef();
	/// destructor
	virtual ~AutoRef();

    /// bump reference
    void Retain();
    /// decrease reference
    void Release();
private:
    int refcount;
}; 

//------------------------------------------------------------------------------
/**
*/
inline
AutoRef::AutoRef() :
    refcount(0)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
AutoRef::~AutoRef()
{
    assert(refcount == 0);
}

//------------------------------------------------------------------------------
/**
*/
inline void 
AutoRef::Retain()
{
    this->refcount++;
}

//------------------------------------------------------------------------------
/**
*/
inline void 
AutoRef::Release()
{
    this->refcount--;
    if (this->refcount == 0)
    {
        delete this;
    }
}

} // namespace Internal
//------------------------------------------------------------------------------