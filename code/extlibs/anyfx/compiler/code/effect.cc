//------------------------------------------------------------------------------
//  effect.cc
//  (C) 2013 Gustav Sterbrant
//------------------------------------------------------------------------------
#include "./effect.h"
#include <assert.h>
#include "constant.h"

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
	unsigned i;
	for (i = 0; i < programs.size(); i++)
	{
		this->programs[i].Destroy();
	}

	for (i = 0; i < variables.size(); i++)
	{
		this->variables[i].Destroy();
	}

	for (i = 0; i < constants.size(); i++)
	{
		this->constants[i].Destroy();
	}

	for (i = 0; i < renderStates.size(); i++)
	{
		this->renderStates[i].Destroy();
	}

	for (i = 0; i < functions.size(); i++)
	{
		this->functions[i].Destroy();
	}

	for (i = 0; i < structures.size(); i++)
	{
		this->structures[i].Destroy();
	}

	for (i = 0; i < varBlocks.size(); i++)
	{
		this->varBlocks[i].Destroy();
	}

	for (i = 0; i < this->varBuffers.size(); i++)
	{
		this->varBuffers[i].Destroy();
	}

	// delete shaders
	std::map<std::string, Shader*>::iterator it;
	for (it = this->shaders.begin(); it != this->shaders.end(); it++)
	{
		delete it->second;
	}
	this->shaders.clear();
	this->programs.clear();
	this->variables.clear();
	this->constants.clear();
	this->renderStates.clear();
	this->functions.clear();
	this->structures.clear();
	this->varBlocks.clear();
	this->varBuffers.clear();
}

//------------------------------------------------------------------------------
/**
*/
void
Effect::AddProgram(const Program& program)
{
	this->programs.push_back(program);
}

//------------------------------------------------------------------------------
/**
*/
void
Effect::AddVariable(const Variable& var)
{
	this->variables.push_back(var);
}

//------------------------------------------------------------------------------
/**
*/
void
Effect::AddConstant(const Constant& constant)
{
	this->constants.push_back(constant);
}

//------------------------------------------------------------------------------
/**
*/
void
Effect::AddRenderState(const RenderState& state)
{
	this->renderStates.push_back(state);
}

//------------------------------------------------------------------------------
/**
*/
void
Effect::AddFunction(const Function& function)
{
	this->functions.push_back(function);
}

//------------------------------------------------------------------------------
/**
*/
void
Effect::AddStructure(const Structure& structure)
{
	this->structures.push_back(structure);
}

//------------------------------------------------------------------------------
/**
*/
void
Effect::AddVarBlock(const VarBlock& varBlock)
{
	this->varBlocks.push_back(varBlock);
}

//------------------------------------------------------------------------------
/**
*/
void
Effect::AddVarBuffer(const VarBuffer& varBuffer)
{
    this->varBuffers.push_back(varBuffer);
}

//------------------------------------------------------------------------------
/**
*/
void
Effect::AddSubroutine(const Subroutine& subroutine)
{
    this->subroutines.push_back(subroutine);
}

//------------------------------------------------------------------------------
/**
*/
void
Effect::AddSampler(const Sampler& sampler)
{
	this->samplers.push_back(sampler);
}

//------------------------------------------------------------------------------
/**
*/
void 
Effect::Setup()
{
	assert(this->header.GetType() != Header::InvalidType);

	// build shaders, this will make sure we have all the shader programs we need, although they are not complete yet
	unsigned i;
	for (i = 0; i < this->programs.size(); i++)
	{
		this->programs[i].BuildShaders(this->header, this->functions, this->shaders);
	}

	// now, remove all functions which are not bound as shaders
	for (i = 0; i < this->functions.size(); i++)
	{
		// get function
		const Function& func = this->functions[i];

		if (func.IsShader())
		{
			this->functions.erase(this->functions.begin() + i);
			i--;
		}
	}

	// create a placeholder render state, which will be used for programs where no render state is explicitly assigned
	this->placeholderRenderState.SetName("PlaceholderState");
    this->placeholderRenderState.SetReserved(true);
    this->renderStates.insert(this->renderStates.begin(), this->placeholderRenderState);

	if (header.GetFlags() & Header::PutGlobalVariablesInBlock)
	{
		this->placeholderVarBlock.SetName("GlobalBlock");
		this->placeholderVarBlock.SetReserved(true);
		this->placeholderVarBlock.AddQualifier("shared");

		for (i = 0; i < this->variables.size(); i++)
		{
			AnyFX::Variable& var = this->variables[i];
			var.Preprocess();
			if (var.GetVarType().GetType() < DataType::Sampler1D && var.IsUniform())
			{
				this->placeholderVarBlock.AddVariable(var);
				this->variables.erase(this->variables.begin() + i);
				i--;
			}

		}
		this->varBlocks.insert(this->varBlocks.begin(), this->placeholderVarBlock);
	}
	else
	{
		for (i = 0; i < this->variables.size(); i++)
		{
			AnyFX::Variable& var = this->variables[i];
			var.Preprocess();
		}
	}
    

	// sort all variables in varblocks
	for (i = 0; i < this->varBlocks.size(); i++)
	{
		this->varBlocks[i].SortVariables();
	}

	// now, finalize all shaders, this adds all structures, varblocks and functions into all shaders, making sure they compile properly
	// we let the target compiler optimize unnecessary variables and functions away
	std::map<std::string, Shader*>::iterator it;
	for (it = this->shaders.begin(); it != this->shaders.end(); it++)
	{
		Shader* shader = it->second;
		shader->Setup();
	}
}

//------------------------------------------------------------------------------
/**
*/
void
Effect::TypeCheck(TypeChecker& typechecker)
{
	this->header.TypeCheck(typechecker);

	// typecheck all shaders
	std::map<std::string, Shader*>::iterator it;
	for (it = this->shaders.begin(); it != this->shaders.end(); it++)
	{
		Shader* shader = it->second;
		shader->TypeCheck(typechecker);
	}

	unsigned i;
    unsigned j = 0;

	for (i = 0; i < this->structures.size(); i++)
	{
		this->structures[i].TypeCheck(typechecker);
	}

    for (i = 0; i < this->subroutines.size(); i++)
    {
        this->subroutines[i].TypeCheck(typechecker);
        this->subroutines[i].UpdateCode(this->header, j++);
    }

    // go through subroutines a second time to check for variables which may have been removed due to optimization
    for (i = 0; i < this->subroutines.size(); i++)
    {
        this->subroutines[i].CheckForOptimization(typechecker);
    }

	for (i = 0; i < this->variables.size(); i++)
	{
		this->variables[i].TypeCheck(typechecker);
	}

	for (i = 0; i < this->samplers.size(); i++)
	{
		this->samplers[i].TypeCheck(typechecker);
	}

	for (i = 0; i < this->renderStates.size(); i++)
	{
		this->renderStates[i].TypeCheck(typechecker);
	}

	for (i = 0; i < this->functions.size(); i++)
	{
		this->functions[i].TypeCheck(typechecker);
		this->functions[i].Restore(this->header, j++);
	}

	for (i = 0; i < this->constants.size(); i++)
	{
		this->constants[i].TypeCheck(typechecker);
	}

	for (i = 0; i < this->varBlocks.size(); i++)
	{
		this->varBlocks[i].TypeCheck(typechecker);
	}

    for (i = 0; i < this->varBuffers.size(); i++)
    {
        this->varBuffers[i].TypeCheck(typechecker);
    }

	for (i = 0; i < this->programs.size(); i++)
	{
		this->programs[i].TypeCheck(typechecker);
	}
}

//------------------------------------------------------------------------------
/**
*/
void
Effect::Generate(Generator& generator)
{
	// typecheck all shaders
	std::map<std::string, Shader*>::iterator it;
	for (it = this->shaders.begin(); it != this->shaders.end(); it++)
	{
		Shader* shader = it->second;

		// generate code for shaders
		shader->Generate(generator, this->variables, this->structures, this->constants, this->varBlocks, this->varBuffers, this->subroutines, this->functions);
	}
}

//------------------------------------------------------------------------------
/**
*/
void
Effect::Compile(BinWriter& writer)
{
	assert(this->header.GetType() != Header::InvalidType);

	// write magic number
	writer.WriteInt('AFX1');

	// write header, vital!
	this->header.Compile(writer);

	unsigned i;
	
	// write FourCC code for shaders
	writer.WriteInt('SHAD');

	// write amount of shaders
	writer.WriteInt(this->shaders.size());

	// compile all shaders
	std::map<std::string, Shader*>::iterator it;
	for (it = this->shaders.begin(); it != this->shaders.end(); it++)
	{
		Shader* shader = it->second;

		// then compile
		shader->Compile(writer);
	}

	// write FourCC code for render states
	writer.WriteInt('RENS');

	// write amount of render states
	writer.WriteInt(this->renderStates.size());

	// compile render states for runtime
	for (i = 0; i < this->renderStates.size(); i++)
	{
		this->renderStates[i].Compile(writer);
	}

    // write FourCC code for subroutines
    writer.WriteInt('SUBR');

    // write amount of subroutines
    writer.WriteInt(this->subroutines.size());

    // compile subroutines for runtime
    for (i = 0; i < this->subroutines.size(); i++)
    {
        this->subroutines[i].Compile(writer);
    }

	// write FourCC code for programs
	writer.WriteInt('PROG');

	// write amount of programs
	writer.WriteInt(this->programs.size());

	// compile programs for runtime
	for (i = 0; i < this->programs.size(); i++)
	{
		this->programs[i].Compile(writer);
	}

	// write FourCC code for variables
	writer.WriteInt('VARI');

	// write amount of variables
	writer.WriteInt(this->variables.size());

	// compile variables for runtime
	for (i = 0; i < this->variables.size(); i++)
	{
		this->variables[i].Compile(writer);
	}

	// write FourCC code for samplers
	writer.WriteInt('SAMP');

	// write amount of samplers
	writer.WriteInt(this->samplers.size());

	for (i = 0; i < this->samplers.size(); i++)
	{
		this->samplers[i].Compile(writer);
	}

	// write FourCC code for varblocks
	writer.WriteInt('VARB');

	// write amount of varblocks
	writer.WriteInt(this->varBlocks.size());

	// compile varblocks for runtime
	for (i = 0; i < this->varBlocks.size(); i++)
	{
		this->varBlocks[i].Compile(writer);
	}

    // write FourCC code for varbuffers
    writer.WriteInt('VRBF');

    // write amount of varbuffers
    writer.WriteInt(this->varBuffers.size());

    // compile varbuffers for runtime
    for (i = 0; i < this->varBuffers.size(); i++)
    {
        this->varBuffers[i].Compile(writer);
    }
}

} // namespace AnyFX
