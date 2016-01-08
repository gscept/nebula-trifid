
#pragma once
//------------------------------------------------------------------------------
/**
    @class AnyFX::Shader
    
	A shader represents a composed target-language representation of a function.

	Its main responsibilities is to format a given function, 
	variable list and external functions, 
	into something a target language shader compiler can understand.
    
    (C) 2013 Gustav Sterbrant
*/
//------------------------------------------------------------------------------
#include <string>
#include "function.h"
#include "variable.h"
#include "header.h"
namespace AnyFX
{
class Variable;
class Structure;
class VarBlock;
class Function;
class Constant;
class VarBuffer;
class Subroutine;
class Shader : public Compileable
{
public:
	
	enum GenerationTarget
	{
		GLSL4,
		GLSL3,
		GLSL2,
		GLSL1,
		HLSL5,
		HLSL4,
		HLSL3,
		PS3,
		PS4,
		WII,
		WIIU,

		NumGenerationTargets
	};

	/// constructor
	Shader();
	/// destructor
	virtual ~Shader();

	/// sets the shader type
	void SetType(const unsigned type);
	/// gets the shader type
	unsigned GetType() const;
	/// sets the function object which this shader will use as its main entry point
	void SetFunction(const Function& func);
	/// gets the function
	const Function& GetFunction() const;
	/// sets shader name
	void SetName(const std::string& name);
	/// gets shader name
	const std::string& GetName() const;
	/// sets the target language, which is necessary to create the formatted code
	void SetHeader(const Header& header);
	/// gets the target language
	const GenerationTarget& GetTargetLanguage() const;
    /// add a compile flag to the shader, separated by pipes ('|')
    void SetCompileFlags(const std::string& flags);

	/// set subroutine mappings
	void SetSubroutineMappings(const std::map<std::string, std::string>& subroutineMappings);

	/// type check shader
	void TypeCheck(TypeChecker& typechecker);
	/// compile shader
	void Compile(BinWriter& writer);
	

	/// sets up shader
	void Setup();
	/// generates code for shader
	void Generate(
		Generator& generator,
		const std::vector<Variable>& vars,
		const std::vector<Structure>& structures,
		const std::vector<Constant>& constants,
		const std::vector<VarBlock>& blocks, 
        const std::vector<VarBuffer>& buffers,
        const std::vector<Subroutine>& subroutines,
		const std::vector<Function>& functions);

private:

#pragma region OpenGL
	/// generates GLSL4 target code
	void GenerateGLSL4(Generator& generator);
	/// generates GLSL3 target code
	void GenerateGLSL3(Generator& generator);

	/// output AnyFX formatted GLSL problem using the AMD/Intel syntax
	void GLSLProblemIntelATI(Generator& generator, std::stringstream& stream);
	/// output AnyFX formatted GLSL problem using the NVIDIA syntax
	void GLSLProblemNvidia(Generator& generator, std::stringstream& stream);
#pragma endregion

#pragma region DirectX
	/// generates HLSL5 target code
	void GenerateHLSL5(Generator& generator);
	/// generates HLSL4 target code
	void GenerateHLSL4(Generator& generator);
	/// generates HLSL3 target code
	void GenerateHLSL3(Generator& generator);
#pragma endregion

	Function func;
	unsigned shaderType;
	GenerationTarget target;
	std::string name;
	std::string formattedCode;
    std::string compileFlags;
	unsigned codeOffset;

	std::string preamble;
    std::map<int, std::pair<std::string, std::string> > indexToFileMap;
	std::map<std::string, std::string> subroutineMappings;
}; 

//------------------------------------------------------------------------------
/**
*/
inline void
Shader::SetFunction(const Function& func)
{
	this->func = func;
}

//------------------------------------------------------------------------------
/**
*/
inline const Function& 
Shader::GetFunction() const
{
	return this->func;
}

//------------------------------------------------------------------------------
/**
*/
inline void 
Shader::SetName( const std::string& name )
{
	this->name = name;
    this->func.SetName(name);
}

//------------------------------------------------------------------------------
/**
*/
inline const std::string& 
Shader::GetName() const
{
	return this->name;
}

//------------------------------------------------------------------------------
/**
*/
inline const Shader::GenerationTarget&
Shader::GetTargetLanguage() const
{
	return this->target;
}

//------------------------------------------------------------------------------
/**
*/
inline void 
Shader::SetType( const unsigned type )
{
	this->shaderType = type;
}

//------------------------------------------------------------------------------
/**
*/
inline unsigned 
Shader::GetType() const
{
	return this->shaderType;
}
//------------------------------------------------------------------------------
/**
*/
inline void
Shader::SetCompileFlags(const std::string& flags)
{
    this->compileFlags = flags;
}

//------------------------------------------------------------------------------
/**
*/
inline void
Shader::SetSubroutineMappings(const std::map<std::string, std::string>& subroutineMappings)
{
	this->subroutineMappings = subroutineMappings;
}


} // namespace AnyFX
//------------------------------------------------------------------------------
