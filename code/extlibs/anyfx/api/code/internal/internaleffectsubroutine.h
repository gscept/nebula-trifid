#pragma once
//------------------------------------------------------------------------------
/**
    @class AnyFX::InternalEffectSubroutine
    
    Internal representation of a subroutine. Is only superficial and purely contains some information about the subroutine.
    The actual way to set subroutines is through the program object.

    
    (C) 2014 Gustav Sterbrant
*/
//------------------------------------------------------------------------------
#include <string>
#include "effectsubroutine.h"
namespace AnyFX
{
class InternalEffectSubroutine
{
public:

	/// constructor
	InternalEffectSubroutine();
	/// destructor
	virtual ~InternalEffectSubroutine();

    /// too bad this has to be public...
    /// returns the name of the subroutine object
    const std::string& GetName() const;

private:
    friend class EffectSubroutine;
    friend class EffectSubroutineStreamLoader;


    /// returns the subroutine type
    const EffectSubroutine::SubroutineType& GetType() const;

    std::string name;
    EffectSubroutine::SubroutineType type;
}; 

//------------------------------------------------------------------------------
/**
*/
inline const std::string& 
InternalEffectSubroutine::GetName() const
{
    return this->name;
}

//------------------------------------------------------------------------------
/**
*/
inline const EffectSubroutine::SubroutineType& 
InternalEffectSubroutine::GetType() const
{
    return this->type;
}

} // namespace AnyFX
//------------------------------------------------------------------------------