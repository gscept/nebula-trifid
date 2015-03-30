//------------------------------------------------------------------------------
//  effect.cc
//  (C) 2013 Gustav Sterbrant
//------------------------------------------------------------------------------
#include "./effect.h"
#include "./effectvarblock.h"
#include "./effectvariable.h"
#include "./effectprogram.h"
#include "./effectshader.h"
#include "./effectrenderstate.h"
#include "./effectsampler.h"
#include "./effectvarbuffer.h"
#include "./effectsubroutine.h"
#include <assert.h>

namespace AnyFX
{

//------------------------------------------------------------------------------
/**
*/
Effect::Effect()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
Effect::~Effect()
{
	// a bit ugly, but then we don't want anyone to accidentally call discard and then try to use the effect.
	this->Discard();
}

//------------------------------------------------------------------------------
/**
*/
unsigned 
Effect::GetNumVariables() const
{
    return this->numVariables;
}

//------------------------------------------------------------------------------
/**
*/
EffectVariable* 
Effect::GetVariableByIndex( unsigned i ) const
{
	assert(this->numVariables > i);
	return this->variablesByIndex[i];
}

//------------------------------------------------------------------------------
/**
*/
EffectVariable* 
Effect::GetVariableByName( const std::string& name )
{
	assert(this->variablesByName.find(name) != this->variablesByName.end());
	return this->variablesByName[name];
}

//------------------------------------------------------------------------------
/**
*/
EffectVariable** 
Effect::GetVariables() const
{
    return this->variablesByIndex;
}

//------------------------------------------------------------------------------
/**
*/
unsigned 
Effect::GetNumPrograms() const
{
    return this->numPrograms;
}

//------------------------------------------------------------------------------
/**
*/
EffectProgram* 
Effect::GetProgramByIndex( unsigned i ) const
{
	assert(this->numPrograms > i);
	return this->programsByIndex[i];
}

//------------------------------------------------------------------------------
/**
*/
EffectProgram* 
Effect::GetProgramByName( const std::string& name )
{
	assert(this->programsByName.find(name) != this->programsByName.end());
	return this->programsByName[name];
}

//------------------------------------------------------------------------------
/**
*/
EffectProgram** 
Effect::GetPrograms() const
{
    return this->programsByIndex;
}

//------------------------------------------------------------------------------
/**
*/
unsigned 
Effect::GetNumShaders() const
{
    return this->numShaders;
}

//------------------------------------------------------------------------------
/**
*/
EffectShader* 
Effect::GetShaderByIndex( unsigned i ) const
{
	assert(this->numShaders > i);
	return this->shadersByIndex[i];
}

//------------------------------------------------------------------------------
/**
*/
EffectShader* 
Effect::GetShaderByName( const std::string& name )
{
	assert(this->shadersByName.find(name) != this->shadersByName.end());
	return this->shadersByName[name];
}

//------------------------------------------------------------------------------
/**
*/
EffectShader** 
Effect::GetShaders() const
{
    return this->shadersByIndex;
}

//------------------------------------------------------------------------------
/**
*/
unsigned 
Effect::GetNumRenderStates() const
{
    return this->numRenderStates;
}

//------------------------------------------------------------------------------
/**
*/
EffectRenderState* 
Effect::GetRenderStateByIndex( unsigned i ) const
{
	assert(this->numRenderStates > i);
	return this->renderStatesByIndex[i];
}

//------------------------------------------------------------------------------
/**
*/
EffectRenderState* 
Effect::GetRenderStateByName( const std::string& name )
{
	assert(this->renderStatesByName.find(name) != this->renderStatesByName.end());
	return this->renderStatesByName[name];
}

//------------------------------------------------------------------------------
/**
*/
EffectRenderState** 
Effect::GetRenderStates() const
{
    return this->renderStatesByIndex;
}

//------------------------------------------------------------------------------
/**
*/
unsigned 
Effect::GetNumVarblocks() const
{
    return this->numVarblocks;
}

//------------------------------------------------------------------------------
/**
*/
EffectVarblock* 
Effect::GetVarblockByIndex( unsigned i ) const
{
	assert(this->numVarblocks > i);
	return this->varblocksByIndex[i];
}

//------------------------------------------------------------------------------
/**
*/
EffectVarblock* 
Effect::GetVarblockByName( const std::string& name )
{
	assert(this->varblocksByName.find(name) != this->varblocksByName.end());
	return this->varblocksByName[name];
}

//------------------------------------------------------------------------------
/**
*/
EffectVarblock** 
Effect::GetVarblocks() const
{
    return this->varblocksByIndex;
}

//------------------------------------------------------------------------------
/**
*/
unsigned 
Effect::GetNumVarbuffers() const
{
    return this->numVarbuffers;
}

//------------------------------------------------------------------------------
/**
*/
EffectVarbuffer* 
Effect::GetVarbufferByIndex( unsigned i ) const
{
    assert(this->numVarbuffers > i);
    return this->varbuffersByIndex[i];
}

//------------------------------------------------------------------------------
/**
*/
EffectVarbuffer* 
Effect::GetVarbufferByName( const std::string& name )
{
    assert(this->varbuffersByName.find(name) != this->varbuffersByName.end());
    return this->varbuffersByName[name];
}

//------------------------------------------------------------------------------
/**
*/
EffectVarbuffer** 
Effect::GetVarbuffers() const
{
    return this->varbuffersByIndex;
}

//------------------------------------------------------------------------------
/**
*/
unsigned 
Effect::GetNumSubroutines() const
{
    return this->numSubroutines;
}

//------------------------------------------------------------------------------
/**
*/
EffectSubroutine* 
Effect::GetSubroutineByIndex( unsigned i ) const
{
    assert(this->numSubroutines > i);
    return this->subroutinesByIndex[i];
}

//------------------------------------------------------------------------------
/**
*/
EffectSubroutine* 
Effect::GetSubroutineByName( const std::string& name )
{
    assert(this->subroutinesByName.find(name) != this->subroutinesByName.end());
    return this->subroutinesByName[name];
}

//------------------------------------------------------------------------------
/**
*/
EffectSubroutine** 
Effect::GetSubroutines() const
{
    return this->subroutinesByIndex;
}

//------------------------------------------------------------------------------
/**
*/
unsigned 
Effect::GetNumSamplers() const
{
    return this->numSamplers;
}

//------------------------------------------------------------------------------
/**
*/
EffectSampler* 
Effect::GetSamplerByIndex( unsigned i ) const
{
	assert(this->numSamplers > i);
	return this->samplersByIndex[i];
}

//------------------------------------------------------------------------------
/**
*/
EffectSampler* 
Effect::GetSamplerByName( const std::string& name )
{
	assert(this->samplersByName.find(name) != this->samplersByName.end());
	return this->samplersByName[name];
}

//------------------------------------------------------------------------------
/**
*/
EffectSampler** 
Effect::GetSamplers() const
{
    return this->samplersByIndex;
}

//------------------------------------------------------------------------------
/**
*/
void 
Effect::Discard()
{
	unsigned i;

	// delete varblocks
	for (i = 0; i < this->numVarblocks; i++)
	{
		this->varblocksByIndex[i]->Discard();
		delete this->varblocksByIndex[i];
	}
	delete [] this->varblocksByIndex;
	this->varblocksByName.clear();

    // delete varbuffers
    for (i = 0; i < this->numVarbuffers; i++)
    {
        this->varbuffersByIndex[i]->Discard();
        delete this->varbuffersByIndex[i];
    }
    delete [] this->varbuffersByIndex;
    this->varbuffersByName.clear();

    // delete subroutines
    for (i = 0; i< this->numSubroutines; i++)
    {
        this->subroutinesByIndex[i]->Discard();
        delete this->subroutinesByIndex[i];
    }
    delete [] this->subroutinesByIndex;
    this->subroutinesByName.clear();

	// delete variables
	for (i = 0; i < this->numVariables; i++)
	{
		this->variablesByIndex[i]->Discard();
		delete this->variablesByIndex[i];
	}
	delete [] this->variablesByIndex;
	this->variablesByName.clear();

	// delete programs
	for (i = 0; i < this->numPrograms; i++)
	{
		this->programsByIndex[i]->Discard();
		delete this->programsByIndex[i];
	}
	delete [] this->programsByIndex;
	this->programsByName.clear();

	// delete shaders
	for (i = 0; i < this->numShaders; i++)
	{
		this->shadersByIndex[i]->Discard();
		delete this->shadersByIndex[i];
	}
	delete [] this->shadersByIndex;
	this->shadersByName.clear();

	// delete render states
	for (i = 0; i < this->numRenderStates; i++)
	{
		this->renderStatesByIndex[i]->Discard();
		delete this->renderStatesByIndex[i];
	}
	delete [] this->renderStatesByIndex;
	this->renderStatesByName.clear();

	// delete samplers
	for (i = 0; i < this->numSamplers; i++)
	{
		this->samplersByIndex[i]->Discard();
		delete this->samplersByIndex[i];
	}
	delete [] this->samplersByIndex;
	this->samplersByName.clear();
}


} // namespace AnyFX
