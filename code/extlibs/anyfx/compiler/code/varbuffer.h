#pragma once
//------------------------------------------------------------------------------
/**
    @class AnyFX::VarBuffer
    
    A VarBuffer denotes an object which acts like a buffered struct. 
    This type of object is bound to the rendering pipeline through a buffer.
    
    (C) 2013 Gustav Sterbrant
*/
//------------------------------------------------------------------------------
#include <string>
#include <vector>
#include "variable.h"
#include "symbol.h"
#include "annotation.h"
namespace AnyFX
{
class VarBuffer : public Symbol
{
public:
	/// constructor
	VarBuffer();
	/// destructor
	virtual ~VarBuffer();

	/// set annotation
	void SetAnnotation(const Annotation& annotation);

	/// add variable
	void AddVariable(const Variable& var);
	/// get variables
	const std::vector<Variable>& GetVariables() const;

	/// get if varblock is shared
	bool IsShared() const;

	/// add qualifier to varbuffer
	void AddQualifier(const std::string& qualifier);
	/// get number of qualifiers
	const unsigned GetNumQualifiers() const;
	/// get qualifier by index
	const std::string& GetQualifier(unsigned i) const;

	/// sorts variables in varblock
	void SortVariables();

	/// type checks var block
	void TypeCheck(TypeChecker& typechecker);
	/// compiles var block
	void Compile(BinWriter& writer);

	/// format variable to fit target language
	std::string Format(const Header& header, const int index) const;
private:
	std::vector<Variable> variables;
	std::vector<std::string> qualifiers;
	bool shared;
	unsigned size;

	bool hasAnnotation;
	Annotation annotation;
}; 

//------------------------------------------------------------------------------
/**
*/
inline void
VarBuffer::SetAnnotation(const Annotation& annotation)
{
    this->annotation = annotation;
    this->hasAnnotation = true;
}

//------------------------------------------------------------------------------
/**
*/
inline void
VarBuffer::AddQualifier(const std::string& qualifier)
{
	this->qualifiers.push_back(qualifier);
}

//------------------------------------------------------------------------------
/**
*/
inline const unsigned
VarBuffer::GetNumQualifiers() const
{
	return this->qualifiers.size();
}

//------------------------------------------------------------------------------
/**
*/
inline const std::string&
VarBuffer::GetQualifier(unsigned i) const
{
	return this->qualifiers[i];
}

//------------------------------------------------------------------------------
/**
*/
inline bool 
VarBuffer::IsShared() const
{
	return this->shared;
}

//------------------------------------------------------------------------------
/**
*/
inline const std::vector<Variable>& 
VarBuffer::GetVariables() const
{
	return this->variables;
}

} // namespace AnyFX
//------------------------------------------------------------------------------
