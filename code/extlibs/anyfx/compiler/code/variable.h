#pragma once
//------------------------------------------------------------------------------
/**
    @class Variable
    
    A variable describes a semi-transparent type declaration, variable name, and default value 
    
    (C) 2013 Gustav Sterbrant
*/
//------------------------------------------------------------------------------
#include <iostream>
#include <vector>
#include "types.h"
#include "datatype.h"
#include "symbol.h"
#include "valuelist.h"
#include "annotation.h"
#include "expressions/expression.h"
namespace AnyFX
{

class Variable : public Symbol
{
public:

	enum ImageFormat
	{
		RGBA32F,
		RGBA16F,
		RG32F,
		RG16F,
		R11G11B10F,
		R32F,
		R16F,
		RGBA16,
		RGB10A2,
		RGBA8,
		RG16,
		RG8,
		R16,
		R8,
		RGBA16SNORM,
		RGBA8SNORM,
		RG16SNORM,
		RG8SNORM,
		R16SNORM,
		R8SNORM,
		RGBA32I,
		RGBA16I,
		RGBA8I,
		RG32I,
		RG16I,
		RG8I,
		R32I,
		R16I,
		R8I,
		RGBA32UI,
		RGBA16UI,
		RGB10A2UI,
		RGBA8UI,
		RG32UI,
		RG16UI,
		RG8UI,
		R32UI,
		R16UI,
		R8UI,
		NoFormat,

		NumImageFormats,

		InvalidImageFormat
	};

	enum AccessMode
	{
		Read,
		Write,
		ReadWrite,
		NoAccess,

		NumImageAccessModes,

		InvalidAccess
	};

	enum ArrayType
	{
		SimpleArray,		// evaluate array size by using value table single entry number of values
		TypedArray,			// evaluate array size by using value table size
		ExplicitArray,		// evaluate array size by expression
        UnsizedArray
	};

    enum QualifierFlags
    {
        NoQualifiers = 0,
        GroupShared = 1 << 0,   // group shared means shared within a shader compute group
        Shared = 1 << 1,        // ordinary shared means shared during the application execution
        Bindless = 1 << 2       // Denotes that a texture should be used as bindless

    };

	/// constructor
	Variable();
	/// destructor
	virtual ~Variable();

    /// set annotation
    void SetAnnotation(const Annotation& annotation);

	/// sets array size expression
	void SetSizeExpression(Expression* expr);
	/// gets array size expression
	Expression* GetSizeExpression() const;

	/// sets array type
	void SetArrayType(const ArrayType& type);
	/// gets array type
	const ArrayType& GetArrayType() const;

	/// gets array sizes
	const int GetArraySize() const;
	/// gets variable byte size
	const unsigned GetByteSize() const;

	/// adds a qualifier to the variable
	void AddQualifier(const std::string& qualifier);
	/// get number of qualifiers
	const unsigned GetNumQualifiers() const;
	/// get qualifier by index
	const std::string& GetQualifier(unsigned i) const;

	/// adds a value-type pair
	void AddValue(const DataType& type, const ValueList& value);
	/// adds a value-type for basic types
	void AddValue(const ValueList& value);

	/// returns true if default value exists
	bool HasDefaultValue() const;

    /// returns true if variable is a subroutine method pointer
    bool IsSubroutine() const;

	/// sets the type of a variable
	void SetVarType(const DataType& type);
	/// get variable type
	const DataType& GetVarType() const;

	/// set variable access mode
	void SetAccess(const AccessMode& mode);
	/// set variable format
	void SetFormat(const ImageFormat& format);

	/// type checks variables
	void TypeCheck(TypeChecker& typechecker);
	/// compiles variable
	void Compile(BinWriter& writer);

	/// format variable to fit target language
	std::string Format(const Header& header, bool inVarblock = false) const;

private:
    friend class VarBlock;

    /// evaluates array size
    void EvaluateArraySize(TypeChecker& typechecker);

	/// converts image format to string based on header
	std::string FormatImageFormat(const Header& header) const;
	/// converts access pattern to string based on header
	std::string FormatImageAccess(const Header& header) const;

	
	std::string defaultValue;
	std::vector<std::string> qualifiers;
	std::vector<std::pair<DataType, ValueList> > valueTable;
	DataType type;
	ImageFormat format;
	AccessMode accessMode;
    int qualifierFlags;

	ArrayType arrayType;
	bool isArray;
    bool isSubroutine;
	int arraySize;
	Expression* sizeExpression;

	bool hasDefaultValue;
	bool hasAnnotation;
	Annotation annotation;
}; 

//------------------------------------------------------------------------------
/**
*/
inline void
Variable::SetAnnotation(const Annotation& annotation)
{
    this->annotation = annotation;
    this->hasAnnotation = true;
}

//------------------------------------------------------------------------------
/**
*/
inline void
Variable::SetSizeExpression(Expression* expr)
{
	this->sizeExpression = expr;
	this->isArray = true;
}

//------------------------------------------------------------------------------
/**
*/
inline Expression* 
Variable::GetSizeExpression() const
{
	return this->sizeExpression;
}

//------------------------------------------------------------------------------
/**
*/
inline void
Variable::SetArrayType(const ArrayType& type)
{
	this->isArray = true;
	this->arrayType = type;
}

//------------------------------------------------------------------------------
/**
*/
inline const Variable::ArrayType& 
Variable::GetArrayType() const
{
	return this->arrayType;
}

//------------------------------------------------------------------------------
/**
*/
inline const int 
Variable::GetArraySize() const
{
	return this->arraySize;
}

//------------------------------------------------------------------------------
/**
*/
inline void
Variable::AddQualifier(const std::string& qualifier)
{
	this->qualifiers.push_back(qualifier);
}

//------------------------------------------------------------------------------
/**
*/
inline const unsigned 
Variable::GetNumQualifiers() const
{
	return this->qualifiers.size();
}

//------------------------------------------------------------------------------
/**
*/
inline const std::string&
Variable::GetQualifier(unsigned i) const
{
	return this->qualifiers[i];
}

//------------------------------------------------------------------------------
/**
*/
inline bool 
Variable::HasDefaultValue() const
{
	return this->hasDefaultValue;
}

//------------------------------------------------------------------------------
/**
*/
inline bool 
Variable::IsSubroutine() const
{
    return this->isSubroutine;
}

//------------------------------------------------------------------------------
/**
*/
inline void
Variable::SetVarType(const DataType& type)
{
	this->type = type;
}

//------------------------------------------------------------------------------
/**
*/
inline const DataType& 
Variable::GetVarType() const
{
	return this->type;
}

//------------------------------------------------------------------------------
/**
*/
inline void
Variable::SetAccess(const AccessMode& mode)
{
	this->accessMode = mode;
}

//------------------------------------------------------------------------------
/**
*/
inline void
Variable::SetFormat(const ImageFormat& format)
{
	this->format = format;
}

} // namespace AnyFX
//------------------------------------------------------------------------------
