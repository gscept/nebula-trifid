#pragma once
//------------------------------------------------------------------------------
/**
    @class Parameter
    
    A parameter is an input or output to or from a function.
    
    (C) 2013 gscept
*/
//------------------------------------------------------------------------------
#include <vector>
#include "datatype.h"
#include "compileable.h"
#include "expressions/expression.h"
namespace AnyFX
{
class Shader;
class Parameter : public Compileable
{
public:

	enum IO
	{
		Input,
		Output,
		InputOutput,
		NoIO,
		
		InvalidIO
	};

	enum Attribute
	{
		DrawInstance,
		Vertex,
		Primitive,
		Invocation,
		Viewport,
		Rendertarget,
		GeometryInstance,
		InnerTessellation,
		OuterTessellation,
		Position,
		PointSize,
		ClipDistance,
		FrontFace,
		Coordinate,
		Depth,
		Color0,
		Color1,
		Color2,
		Color3,
		Color4,
		Color5,
		Color6,
		Color7,
		WorkGroup,
		NumGroups,
		LocalID,
		LocalIndex,
		GlobalID,
		NoAttribute,

		NumAttributes,

		InvalidAttribute
	};

	enum Interpolation
	{
		Smooth,
		Flat,
		NoPerspective,
		Centroid,

		NumInterpolations
	};

	/// constructor
	Parameter();
	/// destructor
	virtual ~Parameter();

	/// sets io mode
	void SetIO(const IO& io);
	/// gets io mode
	const IO& GetIO() const;
	/// set attribute
	void SetAttribute(const Attribute& attr);
	/// get attribute
	const Attribute& GetAttribute() const;
	/// set name
	void SetName(const std::string& name);
	/// get name
	const std::string& GetName() const;
	/// set type
	void SetDataType(const DataType& type);
	/// get type
	const DataType& GetDataType() const;
	/// set parent shader
	void SetShader(Shader* shader);
	/// get parent shader
	Shader* GetShader() const;
	/// gets the patchParam flag
	const bool GetPatchParam() const;
	/// gets const
	bool GetConst() const;
	/// adds qualifier
	void AddQualifier(const std::string& qualifier);
	/// gets number of qualifiers
	unsigned GetNumQualifiers() const;

	/// sets feedback buffer expression
	void SetFeedbackBufferExpression(Expression* expr);
	/// sets feedback buffer offset expression
	void SetFeedbackOffsetExpression(Expression* expr);
	/// returns true if parameter is supposed to be used for transform feedback
	bool IsTransformFeedback() const;
	/// returns to which buffer this parameter should do transform feedback
	const int GetFeedbackBuffer() const;
	/// returns offset into above feedback buffer this parameter is supposed to land
	const unsigned GetFeedbackOffset() const;

	/// returns true if parameter is array
	bool IsArray() const;
	/// returns array size
	unsigned GetArraySize() const;
	/// sets array size expression
	void SetSizeExpression(Expression* expr);
	/// gets array size expression
	Expression* GetSizeExpression() const;
	/// forces the size parameter
	void ForceArrayFlag();

	/// converts attribute to zero-based render target index, asserts if not
	unsigned GetRenderTargetIndex() const;

	/// type checks parameter, throws appropriate errors if attributes doesn't suit the target language
	void TypeCheck(TypeChecker& typechecker);
	/// compiles parameter
	void Compile(BinWriter& writer);

	/// test compatibility between this parameter and another
	bool Compatible(Parameter* other);

	/// formats parameter with specific profile and shader type, the shader type is used to determine different attributes
	std::string Format(const Header& header, unsigned& input, unsigned& output) const;

	/// converts attribute to string
	static std::string AttributeToString(const Attribute& attr);

private:
	/// formats attribute to target-specific string
	std::string FormatAttribute(const Header::Type& type);

	std::vector<std::string> qualifiers;
	bool isConst;

	// array stuff
	Expression* sizeExpression;
	bool isArray;
	unsigned arraySize;

	// feedback buffer stuff
	Expression* feedbackBufferExpression;
	Expression* feedbackOffsetExpression;
	int feedbackBuffer;
	unsigned feedbackOffset;

	bool patchParam;
	IO ioMode;
	Interpolation interpolation;
	Attribute attribute;
	Shader* parentShader;
	std::string name;
	DataType type;
}; 

//------------------------------------------------------------------------------
/**
*/
inline void 
Parameter::SetIO( const IO& io )
{
	this->ioMode = io;
}

//------------------------------------------------------------------------------
/**
*/
inline const Parameter::IO& 
Parameter::GetIO() const
{
	return this->ioMode;
}

//------------------------------------------------------------------------------
/**
*/
inline void 
Parameter::SetAttribute( const Parameter::Attribute& attr )
{
	this->attribute = attr;
}

//------------------------------------------------------------------------------
/**
*/
inline const Parameter::Attribute& 
Parameter::GetAttribute() const
{
	return this->attribute;
}

//------------------------------------------------------------------------------
/**
*/
inline void 
Parameter::SetName( const std::string& name )
{
	this->name = name;
}

//------------------------------------------------------------------------------
/**
*/
inline const std::string& 
Parameter::GetName() const
{
	return this->name;
}

//------------------------------------------------------------------------------
/**
*/
inline void 
Parameter::SetDataType( const DataType& type )
{
	this->type = type;
}

//------------------------------------------------------------------------------
/**
*/
inline const DataType& 
Parameter::GetDataType() const
{
	return this->type;
}

//------------------------------------------------------------------------------
/**
*/
inline void 
Parameter::SetShader( Shader* shader )
{
	this->parentShader = shader;
}

//------------------------------------------------------------------------------
/**
*/
inline Shader* 
Parameter::GetShader() const
{
	return this->parentShader;
}

//------------------------------------------------------------------------------
/**
*/
inline const bool 
Parameter::GetPatchParam() const
{
	return this->patchParam;
}

//------------------------------------------------------------------------------
/**
*/
inline bool 
Parameter::GetConst() const
{
	return this->isConst;
}

//------------------------------------------------------------------------------
/**
*/
inline void 
Parameter::AddQualifier( const std::string& qualifier )
{
	this->qualifiers.push_back(qualifier);
}

//------------------------------------------------------------------------------
/**
*/
inline unsigned 
Parameter::GetNumQualifiers() const
{
	return this->qualifiers.size();
}

//------------------------------------------------------------------------------
/**
*/
inline void
Parameter::SetFeedbackBufferExpression(Expression* expr)
{
	this->feedbackBufferExpression = expr;
}

//------------------------------------------------------------------------------
/**
*/
inline void
Parameter::SetFeedbackOffsetExpression(Expression* expr)
{
	this->feedbackOffsetExpression = expr;
}

//------------------------------------------------------------------------------
/**
*/
inline bool
Parameter::IsTransformFeedback() const
{
	return this->feedbackBuffer > -1;
}

//------------------------------------------------------------------------------
/**
*/
inline const int
Parameter::GetFeedbackBuffer() const
{
	return this->feedbackBuffer;
}

//------------------------------------------------------------------------------
/**
*/
inline const unsigned
Parameter::GetFeedbackOffset() const
{
	return this->feedbackOffset;
}

//------------------------------------------------------------------------------
/**
*/
inline bool 
Parameter::IsArray() const
{
	return this->isArray;
}

//------------------------------------------------------------------------------
/**
*/
inline unsigned 
Parameter::GetArraySize() const
{
	return this->arraySize;
}

//------------------------------------------------------------------------------
/**
*/
inline void 
Parameter::SetSizeExpression( Expression* expr )
{
	this->sizeExpression = expr;
	this->isArray = true;
}

//------------------------------------------------------------------------------
/**
*/
inline Expression* 
Parameter::GetSizeExpression() const
{
	return this->sizeExpression;
}


//------------------------------------------------------------------------------
/**
*/
inline void 
Parameter::ForceArrayFlag()
{
	this->isArray = true;
}

} // namespace AnyFX

//------------------------------------------------------------------------------
