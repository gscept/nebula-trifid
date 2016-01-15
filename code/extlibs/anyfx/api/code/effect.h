#pragma once
//------------------------------------------------------------------------------
/**
    @class AnyFX::Effect
    
    An effect is the main entry point. 
	These are created through the EffectFactory and manage programs, render states, shaders, variables and variable blocks.
    
    (C) 2013 Gustav Sterbrant
*/
//------------------------------------------------------------------------------
#include <map>
#include "EASTL/vector.h"
#include "EASTL/map.h"
#include "EASTL/string.h"
namespace AnyFX
{
class InternalEffectProgram;
class EffectVariable;
class EffectProgram;
class EffectShader;
class EffectRenderState;
class EffectVarblock;
class EffectVarbuffer;
class EffectSubroutine;
class EffectSampler;
class Effect
{
public:

	enum EffectType
	{
		HLSL,
		GLSL,
		PS,
		WII,

		NumInternalEffectTypes
	};

	/// destructor
	virtual ~Effect();

	/// returns number of programs
	unsigned GetNumPrograms() const;
	/// returns program by index
	EffectProgram* GetProgramByIndex(unsigned i) const;
	/// returns program by name
    EffectProgram* GetProgramByName(const eastl::string& name);
	/// returns all programs as a list
	EffectProgram** GetPrograms() const;
    /// returns true if program exists
    bool HasProgram(const eastl::string& name);

	/// returns number of shaders
	unsigned GetNumShaders() const;
	/// returns shader by index
	EffectShader* GetShaderByIndex(unsigned i) const;
	/// returns shader by name
    EffectShader* GetShaderByName(const eastl::string& name);
	/// returns shaders as a list
	EffectShader** GetShaders() const;
    /// returns true if shader exists
    bool HasShader(const eastl::string& name);

	/// returns number of render states
	unsigned GetNumRenderStates() const;
	/// returns render state by index
	EffectRenderState* GetRenderStateByIndex(unsigned i) const;
	/// returns render state by name
    EffectRenderState* GetRenderStateByName(const eastl::string& name);
	/// returns render states as a list
	EffectRenderState** GetRenderStates() const;
    /// returns true if render state exists
    bool HasRenderState(const eastl::string& name);

	/// returns number of variables
	unsigned GetNumVariables() const;
	/// returns variable by index
	EffectVariable* GetVariableByIndex(unsigned i) const;
	/// returns variable by name
    EffectVariable* GetVariableByName(const eastl::string& name);
	/// returns variables as a list
	EffectVariable** GetVariables() const;
    /// returns true if variable exists
    bool HasVariable(const eastl::string& name);

	/// returns number of varblocks
	unsigned GetNumVarblocks() const;
	/// returns varblock by index
	EffectVarblock* GetVarblockByIndex(unsigned i) const;
	/// returns varblock by name
    EffectVarblock* GetVarblockByName(const eastl::string& name);
	/// returns varblocks as a list
	EffectVarblock** GetVarblocks() const;
    /// returns true if varblock exists
    bool HasVarblock(const eastl::string& name);

    /// returns number of varbuffers
    unsigned GetNumVarbuffers() const;
    /// returns varbuffer by index
    EffectVarbuffer* GetVarbufferByIndex(unsigned i) const;
    /// returns varbuffer by name
    EffectVarbuffer* GetVarbufferByName(const eastl::string& name);
    /// returns varbuffer as a list
    EffectVarbuffer** GetVarbuffers() const;
    /// returns true if varbuffer exists
    bool HasVarbuffer(const eastl::string& name);

    /// returns number of subroutine
    unsigned GetNumSubroutines() const;
    /// returns subroutine by index
    EffectSubroutine* GetSubroutineByIndex(unsigned i) const;
    /// returns subroutine by name
    EffectSubroutine* GetSubroutineByName(const eastl::string& name);
    /// returns list of subroutines
    EffectSubroutine** GetSubroutines() const;
    /// returns true if subroutine exists
    bool HasSubroutine(const eastl::string& name);

	/// returns the number of samplers
	unsigned GetNumSamplers() const;
	/// returns sampler by index
	EffectSampler* GetSamplerByIndex(unsigned i) const;
	/// returns sampler by name
    EffectSampler* GetSamplerByName(const eastl::string& name);
	/// returns samplers as list
	EffectSampler** GetSamplers() const;
    /// returns true if sampler exists
    bool HasSampler(const eastl::string& name);

	/// get type
	const Effect::EffectType& GetType() const;
	/// get major version number
	int GetMajor() const;
	/// get minor version number
	int GetMinor() const;

private:
	friend class EffectStreamLoader;
	friend class InternalEffectProgram;

	/// constructor, private since it should be created by the effect factory
	Effect();

	/// callback when effect is done loading
	void LoadingDone();
	/// cleans up effect
	void Discard();

	EffectVariable** variablesByIndex;
	size_t numVariables;
    eastl::map<eastl::string, EffectVariable*> variablesByName;
	
	EffectProgram** programsByIndex;
	size_t numPrograms;
    eastl::map<eastl::string, EffectProgram*> programsByName;

	EffectShader** shadersByIndex;
	size_t numShaders;
    eastl::map<eastl::string, EffectShader*> shadersByName;

	EffectRenderState** renderStatesByIndex;
	size_t numRenderStates;
    eastl::map<eastl::string, EffectRenderState*> renderStatesByName;

	EffectVarblock** varblocksByIndex;
	size_t numVarblocks;
    eastl::map<eastl::string, EffectVarblock*> varblocksByName;

    EffectVarbuffer** varbuffersByIndex;
    size_t numVarbuffers;
    eastl::map<eastl::string, EffectVarbuffer*> varbuffersByName;

    EffectSubroutine** subroutinesByIndex;
    size_t numSubroutines;
    eastl::map<eastl::string, EffectSubroutine*> subroutinesByName;

	EffectSampler** samplersByIndex;
	size_t numSamplers;
    eastl::map<eastl::string, EffectSampler*> samplersByName;

	// integer used to hold pointer to context object, if any exists
	void* contextHandle;
	EffectType type;
	int major;
	int minor;
}; 

//------------------------------------------------------------------------------
/**
*/
inline const Effect::EffectType& 
Effect::GetType() const
{
	return this->type;
}

//------------------------------------------------------------------------------
/**
*/
inline int
Effect::GetMajor() const
{
	return this->major;
}

//------------------------------------------------------------------------------
/**
*/
inline int
Effect::GetMinor() const
{
	return this->minor;
}

} // namespace AnyFX
//------------------------------------------------------------------------------