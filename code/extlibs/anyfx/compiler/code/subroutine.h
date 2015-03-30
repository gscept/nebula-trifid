#pragma once
//------------------------------------------------------------------------------
/**
    @class AnyFX::Subroutine
    
    A subroutine describes a function which can be dynamically bound during runtime.
    
    (C) 2014 Gustav Sterbrant
*/
//------------------------------------------------------------------------------
#include <vector>
#include "symbol.h"
#include "datatype.h"
#include "parameter.h"
#include "function.h"
namespace AnyFX
{
class Subroutine : public Symbol
{
    
public:

    enum SubroutineType
    {
        Signature,
        Implementation
    };

	/// constructor
	Subroutine();
	/// destructor
	virtual ~Subroutine();

    /// set type
    void SetSubroutineType(const SubroutineType& type);
    /// get subroutine type
    const SubroutineType& GetSubroutineType() const;
    
    /// set return type (only viable for signature)
    void SetReturnType(const DataType& type);
    /// get return type
    const DataType& GetReturnType() const;

    /// set signature
    void SetSignature(const std::string& signature);
    /// get signature
    const std::string& GetSignature() const;

    /// set vector of function parameters (only viable for signature)
    void SetParameters(const std::vector<Parameter>& parameters);
    /// get number of parameters
    const unsigned GetNumParameters() const;
    /// get pointer to parameter at index
    Parameter* GetParameter(unsigned index);

    /// sets function
    void SetFunction(const Function& func);
    /// get function
    const Function& GetFunction() const;

    /// rewrite subroutine by replacing function code with subroutine header, and injecting line identifier (only viable on implementation subroutines)
    void UpdateCode(const Header& header, unsigned fileIndex);

    /// typechecks subroutine
    void TypeCheck(TypeChecker& typechecker);
    /// compiles subroutine
    void Compile(BinWriter& writer);
    
    /// called after type checking is done to ensure no parameter is unused
    void CheckForOptimization(TypeChecker& typeChecker);

    /// return the index of the file, basically, this is used with preprocessors to map a number to an actual physical file
    const int GetFileIndex() const;

    /// format subroutine to fit target language
    std::string Format(const Header& header) const;

private:
    /// tags a parameter as used, only use index
    void ParameterUsed(unsigned i);

    DataType returnType;
    SubroutineType subroutineType;
    Function func;
    std::vector<Parameter> parameters;
    std::vector<bool> optimizationList;  // contains list of parameters which were optimized away by this subroutine
    std::string signature;
    int fileIndex;
}; 

//------------------------------------------------------------------------------
/**
*/
inline void 
Subroutine::SetSubroutineType( const SubroutineType& type )
{
    this->subroutineType = type;
}

//------------------------------------------------------------------------------
/**
*/
inline const Subroutine::SubroutineType& 
Subroutine::GetSubroutineType() const
{
    return this->subroutineType;
}

//------------------------------------------------------------------------------
/**
*/
inline void 
Subroutine::SetReturnType( const DataType& type )
{
    this->returnType = type;
}

//------------------------------------------------------------------------------
/**
*/
inline const DataType& 
Subroutine::GetReturnType() const
{
    return this->returnType;
}

//------------------------------------------------------------------------------
/**
*/
inline void 
Subroutine::SetSignature( const std::string& signature )
{
    this->signature = signature;
}

//------------------------------------------------------------------------------
/**
*/
inline const std::string& 
Subroutine::GetSignature() const
{
    return this->signature;
}

//------------------------------------------------------------------------------
/**
*/
inline void 
Subroutine::SetParameters( const std::vector<Parameter>& parameters )
{
    this->parameters = parameters;
}

//------------------------------------------------------------------------------
/**
*/
inline const unsigned 
Subroutine::GetNumParameters() const
{
    return this->parameters.size();
}

//------------------------------------------------------------------------------
/**
*/
inline Parameter* 
Subroutine::GetParameter( unsigned index )
{
    return &this->parameters[index];
}

//------------------------------------------------------------------------------
/**
*/
inline void 
Subroutine::SetFunction( const Function& func )
{
    this->func = func;
}

//------------------------------------------------------------------------------
/**
*/
inline const Function& 
Subroutine::GetFunction() const
{
    return this->func;
}

//------------------------------------------------------------------------------
/**
*/
inline const int 
Subroutine::GetFileIndex() const
{
    return this->fileIndex;
}

} // namespace AnyFX
//------------------------------------------------------------------------------