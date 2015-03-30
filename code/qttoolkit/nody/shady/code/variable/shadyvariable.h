#pragma once
//------------------------------------------------------------------------------
/**
    @class Shady::ShadyVariable
    
    A special type of variable used for shader generation.

    The main difference is that shady variables contains which 
    compiler flags should be set if this variable gets used by the pipeline. 

    This is mainly used for super variations, but can also be used by ordinary nodes as well.

    An example for how this could be used is through defining what function should be used in the event an instance of this variable is not attached.
    
    (C) 2014 Gustav Sterbrant
*/
//------------------------------------------------------------------------------
#include "core/refcounted.h"
#include "variable/variable.h"
#include "shadydefs.h"
namespace Shady
{
class ShadyVariable : public Nody::Variable
{
	__DeclareClass(ShadyVariable);
public:
    enum Result
    {
        VertexResult,
        HullResult,
        DomainResult,
        GeometryResult,
        PixelResult,  
        UnknownResult,

        NumFunction
    };

    enum VarFlags
    {
        NoFlags = 0,
        Constant = 1 << 0,
        Parameter = 1 << 1,
        Static = 1 << 2,

        NumVarFlags = (1 << 3) - 1
    };

	/// constructor
	ShadyVariable();
	/// destructor
	virtual ~ShadyVariable();

    /// set defines
    void SetDefines(const Util::String& defines);
    /// get defines
    const Util::String& GetDefines() const;

    /// set result of input, only viable for variable in a super variation
    void SetResult(const Result& result);
    /// get result of input, only viable for variable in a super variation
    const Result& GetResult() const;

    /// sets source code for given shader language
    void SetSource(const Util::String& code, Shady::Language language);
    /// returns source for given language
    const Util::String& GetSource(Shady::Language language);

    /// converts string to result
    static Result ResultFromString(const Util::String& string);

private:
    Util::String sourceVersions[NumLanguages];
    Result inputResult;
    Util::String defines;
}; 

//------------------------------------------------------------------------------
/**
*/
inline void 
ShadyVariable::SetDefines( const Util::String& defines )
{
    this->defines = defines;
}

//------------------------------------------------------------------------------
/**
*/
inline const Util::String& 
ShadyVariable::GetDefines() const
{
    return this->defines;
}

//------------------------------------------------------------------------------
/**
*/
inline void 
ShadyVariable::SetResult( const ShadyVariable::Result& result )
{
    this->inputResult = result;
}

//------------------------------------------------------------------------------
/**
*/
inline const ShadyVariable::Result& 
ShadyVariable::GetResult() const
{
    return this->inputResult;
}
//------------------------------------------------------------------------------
/**
*/
inline void 
ShadyVariable::SetSource( const Util::String& code, Shady::Language language )
{
    this->sourceVersions[language] = code;
}

//------------------------------------------------------------------------------
/**
*/
inline const Util::String& 
ShadyVariable::GetSource( Shady::Language language )
{
    return this->sourceVersions[language];
}

} // namespace Shady
//------------------------------------------------------------------------------