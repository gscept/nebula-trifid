//------------------------------------------------------------------------------
//  glsl4effectprogram.cc
//  (C) 2013 Gustav Sterbrant
//------------------------------------------------------------------------------
#include "glsl4effectprogram.h"
#include "internal/internaleffectrenderstate.h"
#include "internal/internaleffectsubroutine.h"
#include "internal/internaleffectvariable.h"
#include "glsl4effectshader.h"
#include <assert.h>
#include <algorithm>

namespace AnyFX
{

unsigned GLSL4GlobalProgramState::program = -1;
unsigned GLSL4GlobalProgramState::patchSize = -1;
unsigned* GLSL4GlobalProgramState::vsSubroutines = 0;
unsigned* GLSL4GlobalProgramState::hsSubroutines = 0;
unsigned* GLSL4GlobalProgramState::dsSubroutines = 0;
unsigned* GLSL4GlobalProgramState::gsSubroutines = 0;
unsigned* GLSL4GlobalProgramState::psSubroutines = 0;
unsigned* GLSL4GlobalProgramState::csSubroutines = 0;

//------------------------------------------------------------------------------
/**
*/
GLSL4EffectProgram::GLSL4EffectProgram() :
	programHandle(0)
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
GLSL4EffectProgram::~GLSL4EffectProgram()
{
	// empty
}

//------------------------------------------------------------------------------
/**
    FIXME! 
    Make it so we can apply a program without changing the actual OpenGL program, but instead simply change the subroutines!
*/
void 
GLSL4EffectProgram::Apply()
{
    if (GLSL4GlobalProgramState::program != this->programHandle)
    {
        GLSL4GlobalProgramState::program = this->programHandle;

        // activate GL program
        glUseProgram(this->programHandle);
    }	

    // first time, we must apply all subroutines since
    this->ApplySubroutines();

	// if we support tessellation, then set the patch vertices parameter also
	if (this->supportsTessellation)
	{
        if (GLSL4GlobalProgramState::patchSize != this->patchSize)
        {
            GLSL4GlobalProgramState::patchSize = this->patchSize;
            glPatchParameteri(GL_PATCH_VERTICES, this->patchSize);
        }		
	}

	// unbind all samplers
	//glBindSamplers(0, 64, NULL);

	// apply internal program
	InternalEffectProgram::Apply();
}

//------------------------------------------------------------------------------
/**
*/
void 
GLSL4EffectProgram::ApplySubroutines()
{
    // apply subroutines
    if (GLSL4GlobalProgramState::vsSubroutines != this->vsSubroutineBindings)
    {
        GLSL4GlobalProgramState::vsSubroutines = this->vsSubroutineBindings;
        if (this->numVsSubroutines > 0) glUniformSubroutinesuiv(GL_VERTEX_SHADER,           this->numVsSubroutines, this->vsSubroutineBindings);
    }

    if (GLSL4GlobalProgramState::hsSubroutines != this->hsSubroutineBindings)
    {
        GLSL4GlobalProgramState::hsSubroutines = this->hsSubroutineBindings;
        if (this->numHsSubroutines > 0) glUniformSubroutinesuiv(GL_TESS_CONTROL_SHADER,     this->numHsSubroutines, this->hsSubroutineBindings);
    }

    if (GLSL4GlobalProgramState::dsSubroutines != this->dsSubroutineBindings)
    {
        GLSL4GlobalProgramState::dsSubroutines = this->dsSubroutineBindings;
        if (this->numDsSubroutines > 0) glUniformSubroutinesuiv(GL_TESS_EVALUATION_SHADER,  this->numDsSubroutines, this->dsSubroutineBindings);
    }

    if (GLSL4GlobalProgramState::gsSubroutines != this->gsSubroutineBindings)
    {
        GLSL4GlobalProgramState::gsSubroutines = this->gsSubroutineBindings;
        if (this->numGsSubroutines > 0) glUniformSubroutinesuiv(GL_GEOMETRY_SHADER,         this->numGsSubroutines, this->gsSubroutineBindings);
    }

    if (GLSL4GlobalProgramState::psSubroutines != this->psSubroutineBindings)
    {
        GLSL4GlobalProgramState::psSubroutines = this->psSubroutineBindings;
        if (this->numPsSubroutines > 0) glUniformSubroutinesuiv(GL_FRAGMENT_SHADER,         this->numPsSubroutines, this->psSubroutineBindings);
    }

    if (GLSL4GlobalProgramState::csSubroutines != this->csSubroutineBindings)
    {
        GLSL4GlobalProgramState::csSubroutines = this->csSubroutineBindings;
        if (this->numCsSubroutines > 0) glUniformSubroutinesuiv(GL_COMPUTE_SHADER,          this->numCsSubroutines, this->csSubroutineBindings);
    }    
}

//------------------------------------------------------------------------------
/**
*/
void
GLSL4EffectProgram::SetupSlave(InternalEffectProgram* other)
{
	GLSL4EffectProgram* otherProgram = static_cast<GLSL4EffectProgram*>(other);
	this->programHandle = otherProgram->programHandle;
}

//------------------------------------------------------------------------------
/**
*/
bool 
GLSL4EffectProgram::Link()
{
	// create program
	this->programHandle = glCreateProgram();

	bool needsLinking = false;

	// attach shaders
	if (0 != this->shaderBlock.vs)
	{
		GLSL4EffectShader* glsl4Shader = static_cast<GLSL4EffectShader*>(this->shaderBlock.vs);
		glAttachShader(this->programHandle, glsl4Shader->GetShaderHandle());
		needsLinking = true;
	}

	if (0 != this->shaderBlock.hs)
	{
		GLSL4EffectShader* glsl4Shader = static_cast<GLSL4EffectShader*>(this->shaderBlock.hs);
		glAttachShader(this->programHandle, glsl4Shader->GetShaderHandle());
		needsLinking = true;
	}

	if (0 != this->shaderBlock.ds)
	{
		GLSL4EffectShader* glsl4Shader = static_cast<GLSL4EffectShader*>(this->shaderBlock.ds);
		glAttachShader(this->programHandle, glsl4Shader->GetShaderHandle());
		needsLinking = true;
	}

	if (0 != this->shaderBlock.gs)
	{
		GLSL4EffectShader* glsl4Shader = dynamic_cast<GLSL4EffectShader*>(this->shaderBlock.gs);
		glAttachShader(this->programHandle, glsl4Shader->GetShaderHandle());
		needsLinking = true;
	}

	if (0 != this->shaderBlock.ps)
	{
		GLSL4EffectShader* glsl4Shader = static_cast<GLSL4EffectShader*>(this->shaderBlock.ps);
		glAttachShader(this->programHandle, glsl4Shader->GetShaderHandle());
		needsLinking = true;
	}

	if (0 != this->shaderBlock.cs)
	{
		GLSL4EffectShader* glsl4Shader = static_cast<GLSL4EffectShader*>(this->shaderBlock.cs);
		glAttachShader(this->programHandle, glsl4Shader->GetShaderHandle());
		needsLinking = true;
	}

	if (needsLinking)
	{
		// link program
		glLinkProgram(this->programHandle);

		GLint status;
		glGetProgramiv(this->programHandle, GL_LINK_STATUS, &status);
		GLint size;
		glGetProgramiv(this->programHandle, GL_INFO_LOG_LENGTH, &size);

		if (size > 0)
		{
			GLchar* log = new GLchar[size];
			glGetProgramInfoLog(this->programHandle, size, NULL, log);
			if (status != GL_TRUE)
			{
				// create error string
				this->error = std::string(log, size);
				delete[] log;

				// output false
				return false;
			}
			else
			{
				// set warning flag
				this->warning = std::string(log, size);
				delete[] log;
			}
		}
	}
	else
	{
		// set to default program
		glDeleteProgram(this->programHandle);
		this->programHandle = 0;

        // return false since the program is empty
        return false;
	}

	return true;
}
//------------------------------------------------------------------------------
/**
    sort hook for subroutine uniforms
*/
bool
SubroutineBindingCompare(const std::pair<GLuint, GLuint>& v1, const std::pair<GLuint, GLuint>& v2)
{
	return v1.first < v2.first;
}

//------------------------------------------------------------------------------
/**
    Hmm, perhaps move this to the compile phase?
*/
void 
GLSL4EffectProgram::SetupSubroutines()
{
    assert(0 != this->programHandle);
    this->SetupSubroutineHelper(GL_VERTEX_SHADER,           this->numVsSubroutines, &this->vsSubroutineBindings, this->shaderBlock.vsSubroutines);
    this->SetupSubroutineHelper(GL_TESS_CONTROL_SHADER,     this->numHsSubroutines, &this->hsSubroutineBindings, this->shaderBlock.hsSubroutines);
    this->SetupSubroutineHelper(GL_TESS_EVALUATION_SHADER,  this->numDsSubroutines, &this->dsSubroutineBindings, this->shaderBlock.dsSubroutines);
    this->SetupSubroutineHelper(GL_GEOMETRY_SHADER,         this->numGsSubroutines, &this->gsSubroutineBindings, this->shaderBlock.gsSubroutines);
    this->SetupSubroutineHelper(GL_FRAGMENT_SHADER,         this->numPsSubroutines, &this->psSubroutineBindings, this->shaderBlock.psSubroutines);
    this->SetupSubroutineHelper(GL_COMPUTE_SHADER,          this->numCsSubroutines, &this->csSubroutineBindings, this->shaderBlock.csSubroutines);
}

//------------------------------------------------------------------------------
/**
*/
void 
GLSL4EffectProgram::SetupSubroutineHelper( GLenum shaderType, GLsizei& numBindings, GLuint** bindingArray, const std::map<std::string, InternalEffectSubroutine*>& bindings )
{
    std::map<std::string, InternalEffectSubroutine*>::const_iterator it;

    eastl::vector<std::pair<GLuint, GLuint> > intermediateMap;
    unsigned numActiveSubroutines = 0;
    for (it = bindings.begin(); it != bindings.end(); it++)
    {
        std::string var = (*it).first;
        InternalEffectSubroutine* imp = (*it).second;

        GLint subroutineIndex = glGetSubroutineIndex(this->programHandle, shaderType, imp->GetName().c_str());
        GLint uniformIndex = glGetSubroutineUniformLocation(this->programHandle, shaderType, var.c_str());

        if (uniformIndex != -1 && subroutineIndex != -1)
        {
            intermediateMap.push_back(std::pair<GLuint, GLuint>(uniformIndex, subroutineIndex));
            numActiveSubroutines++;
        }
    }

    GLint numRoutines;
    glGetProgramStageiv(this->programHandle, shaderType, GL_ACTIVE_SUBROUTINE_UNIFORM_LOCATIONS, &numRoutines);
    assert(numActiveSubroutines == numRoutines);

    (*bindingArray) = new GLuint[numActiveSubroutines];
    std::sort(intermediateMap.begin(), intermediateMap.end(), SubroutineBindingCompare);
    unsigned i;
    for (i = 0; i < numActiveSubroutines; i++)
    {
        (*bindingArray)[i] = intermediateMap[i].second;
    }

    numBindings = numActiveSubroutines;
}
} // namespace AnyFX
