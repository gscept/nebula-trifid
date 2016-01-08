#pragma once
//------------------------------------------------------------------------------
/**
    @class AnyFX::VarBlock
    
    A variable block describes an updatable set of variables.
    
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
class VarBlock : public Symbol
{
public:
	/// constructor
	VarBlock();
	/// destructor
	virtual ~VarBlock();

	/// set annotation
	void SetAnnotation(const Annotation& annotation);

	/// add variable
	void AddVariable(const Variable& var);
	/// get variables
	const std::vector<Variable>& GetVariables() const;

	/// get if varblock is shared
	bool IsShared() const;

	/// add qualifier to varblock
	void AddQualifier(const std::string& qualifier);
	/// get number of qualifiers
	const unsigned GetNumQualifiers() const;
	/// get qualifier by index
	const std::string& GetQualifier(unsigned i) const;
    
    /// set backing buffer expression
    void SetBufferExpression(Expression* expr);

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
	bool noSync;
    unsigned bufferCount;

	bool hasAnnotation;
	Annotation annotation;
    Expression* bufferExpression;
}; 

//------------------------------------------------------------------------------
/**
*/
inline void
VarBlock::SetAnnotation(const Annotation& annotation)
{
    this->annotation = annotation;
    this->hasAnnotation = true;
}

//------------------------------------------------------------------------------
/**
*/
inline void 
VarBlock::SetBufferExpression(Expression* expr)
{
    this->bufferExpression = expr;
}

//------------------------------------------------------------------------------
/**
*/
inline bool 
VarBlock::IsShared() const
{
	return this->shared;
}

//------------------------------------------------------------------------------
/**
*/
inline void
VarBlock::AddQualifier(const std::string& qualifier)
{
	this->qualifiers.push_back(qualifier);
}

//------------------------------------------------------------------------------
/**
*/
inline const unsigned
VarBlock::GetNumQualifiers() const
{
	return this->qualifiers.size();
}

//------------------------------------------------------------------------------
/**
*/
inline const std::string&
VarBlock::GetQualifier(unsigned i) const
{
	return this->qualifiers[i];
}

//------------------------------------------------------------------------------
/**
*/
inline const std::vector<Variable>& 
VarBlock::GetVariables() const
{
	return this->variables;
}

} // namespace AnyFX
//------------------------------------------------------------------------------
