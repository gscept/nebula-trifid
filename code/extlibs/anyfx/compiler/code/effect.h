#pragma once
//------------------------------------------------------------------------------
/**
    @class Effect
    
    Main parser entry point, contains complete effect.
    
    (C) 2013 Gustav Sterbrant
*/
//------------------------------------------------------------------------------
#include <vector>
#include <map>
#include "header.h"
#include "program.h"
#include "variable.h"
#include "renderstate.h"
#include "function.h"
#include "structure.h"
#include "varblock.h"
#include "varbuffer.h"
#include "subroutine.h"
#include "compileable.h"
#include "sampler.h"

namespace AnyFX
{

class Effect : public Compileable
{
public:

	/// constructor
	Effect();
	/// destructor
	virtual ~Effect();

	/// sets the header object
	void SetHeader(const Header& header);
	/// gets the header object
	const Header& GetHeader() const;

	/// add program definition
	void AddProgram(const Program& program);
	/// add variable definition
	void AddVariable(const Variable& var);
	/// add constant definition
	void AddConstant(const Constant& constant);
	/// add render state
	void AddRenderState(const RenderState& state);
	/// add function
	void AddFunction(const Function& function);
	/// add structure
	void AddStructure(const Structure& structure);
	/// add varblock
	void AddVarBlock(const VarBlock& varBlock);
    /// add varbuffer
    void AddVarBuffer(const VarBuffer& varBuffer);
    /// add subroutine
    void AddSubroutine(const Subroutine& subroutine);
	/// add sampler
	void AddSampler(const Sampler& sampler);

	/// sets up effect using given header
	void Setup();
	/// type check effect
	void TypeCheck(TypeChecker& typechecker);
	/// generates code
	void Generate(Generator& generator);
	/// compile effect
	void Compile(BinWriter& writer);

private:
	Header header;
	std::vector<Program> programs;
	std::vector<Variable> variables;
	std::vector<Constant> constants;
	std::vector<RenderState> renderStates;
	std::vector<Function> functions;
	std::vector<Structure> structures;
	std::vector<VarBlock> varBlocks;
    std::vector<VarBuffer> varBuffers;
    std::vector<Subroutine> subroutines;
	std::vector<Sampler> samplers;

	std::map<std::string, Shader*> shaders;

	RenderState placeholderRenderState;
    VarBlock placeholderVarBlock;
}; 

//------------------------------------------------------------------------------
/**
*/
inline void 
Effect::SetHeader( const Header& header )
{
	this->header = header;
}

//------------------------------------------------------------------------------
/**
*/
inline const Header&
Effect::GetHeader() const
{
	return this->header;
}

} // namespace AnyFX

//------------------------------------------------------------------------------