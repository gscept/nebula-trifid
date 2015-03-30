#pragma once
//------------------------------------------------------------------------------
/**
    @class AnyFX::EffectSubroutine
    
    Supplies a user interface to a subroutine.
    
    (C) 2014 Gustav Sterbrant
*/
//------------------------------------------------------------------------------

#ifdef AFX_API
#error "afxapi.h included before effectsubroutine.h"
#endif

#include <string>
namespace AnyFX
{
class InternalEffectSubroutine;
class EffectSubroutine
{
public:
    enum SubroutineType

    {
        Signature,
        Implementation,
        Variable
    };

	/// constructor
	EffectSubroutine();
	/// destructor
	virtual ~EffectSubroutine();

    /// discard subroutine
    void Discard();

    /// get subroutine object name
    const std::string& GetName() const;
    /// get subroutine object type
    const SubroutineType& GetType() const;

private:
    friend class EffectSubroutineStreamLoader;
    friend class EffectProgramStreamLoader;

    InternalEffectSubroutine* internalSubroutine;
}; 
} // namespace AnyFX
//------------------------------------------------------------------------------