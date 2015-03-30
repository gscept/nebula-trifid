#pragma once
//------------------------------------------------------------------------------
/**
    @class AnyFX::InternalEffectProgram
    
    EffectProgram backend, inherit this class to provide an implementation
    
    (C) 2013 Gustav Sterbrant
*/
//------------------------------------------------------------------------------
#include "EASTL/vector.h"
#include <string>
#include <map>
namespace AnyFX
{
class Effect;
class InternalEffectRenderState;
class InternalEffectShader;
class InternalEffectSubroutine;
class InternalEffectVariable;
class InternalEffectProgram
{
public:

	enum LinkState
	{
		NotLinked,
		LinkedOk,
		LinkerError
	};
	/// constructor
	InternalEffectProgram();
	/// destructor
	virtual ~InternalEffectProgram();

protected:
	friend class EffectProgram;
	friend class EffectProgramStreamLoader;

	/// applies program, override in subclass to provide functionality
	virtual void Apply();
	/// commits variables
	virtual void Commit();
    /// performs pre-draw stuff
    virtual void PreDraw();
    /// performs after-draw stuff
    virtual void PostDraw();
	/// links program, override in subclass if needed
	virtual bool Link();
	/// returns true if program supports tessellation
	const bool SupportsTessellation() const;

	/// copy internal handle from other program
	virtual void SetupSlave(InternalEffectProgram* other);

    /// sets up subroutine mappings
    virtual void SetupSubroutines();

	/// returns name of program
	const std::string& GetName() const;
	/// returns error, if any
	const std::string& GetError() const;
	/// returns warning string
	const std::string& GetWarning() const;

	InternalEffectRenderState* renderState;
	Effect* effect;

	struct InternalEffectShaderBlock
	{
		InternalEffectShader* vs;		                                                // vertex shader
        std::map<std::string, InternalEffectSubroutine*> vsSubroutines;     // vertex shader
		InternalEffectShader* ps;		                                                // pixel shader
        std::map<std::string, InternalEffectSubroutine*> psSubroutines;     // pixel shader
		InternalEffectShader* ds;		                                                // domain shader
        std::map<std::string, InternalEffectSubroutine*> dsSubroutines;     // domain shader
		InternalEffectShader* hs;		                                                // hull shader
        std::map<std::string, InternalEffectSubroutine*> hsSubroutines;     // hull shader
		InternalEffectShader* gs;		                                                // geometry shader
        std::map<std::string, InternalEffectSubroutine*> gsSubroutines;     // geometry shader
		InternalEffectShader* cs;		                                                // compute shader
        std::map<std::string, InternalEffectSubroutine*> csSubroutines;     // compute shader
	} shaderBlock;

	std::string name;
	std::string error;
	std::string warning;

	LinkState linkState;
	bool supportsTessellation;
    bool supportsTransformFeedback;
	unsigned patchSize;
}; 

//------------------------------------------------------------------------------
/**
*/
inline const std::string& 
InternalEffectProgram::GetName() const
{
	return this->name;
}

//------------------------------------------------------------------------------
/**
*/
inline const std::string& 
InternalEffectProgram::GetError() const
{
	return this->error;
}

//------------------------------------------------------------------------------
/**
*/
inline const std::string&
InternalEffectProgram::GetWarning() const
{
	return this->warning;
}

//------------------------------------------------------------------------------
/**
*/
inline const bool 
InternalEffectProgram::SupportsTessellation() const
{
	return this->supportsTessellation;
}

} // namespace AnyFX
//------------------------------------------------------------------------------
