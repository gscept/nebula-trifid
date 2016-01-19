//------------------------------------------------------------------------------
//  glsl4effectprogram.cc
//  (C) 2013 Gustav Sterbrant
//------------------------------------------------------------------------------
#include "glsl4effectprogram.h"
#include "internal/internaleffectrenderstate.h"
#include "internal/internaleffectsubroutine.h"
#include "internal/internaleffectvariable.h"
#include "internal/internaleffectprogram.h"
#include "glsl4effectshader.h"
#include "glsl4effectvariable.h"
#include "glsl4effectvarbuffer.h"
#include "glsl4effectvarblock.h"
#include <assert.h>
#include "EASTL/sort.h"

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
#if GL4_MULTIBIND
	, varblockBindsCount(0),
	varbufferBindsCount(0),
	textureBindsCount(0),
	imageBindsCount(0),
	varblockRangeBindBuffers(NULL),
	varblockRangeBindOffsets(NULL),
	varblockRangeBindSizes(NULL),
	varbufferRangeBindBuffers(NULL),
	varbufferRangeBindOffsets(NULL),
	varbufferRangeBindSizes(NULL),
	textureBinds(NULL),
	imageBinds(NULL)
#endif
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
GLSL4EffectProgram::~GLSL4EffectProgram()
{
	this->glsl4Variables.clear();
	this->glsl4Varblocks.clear();
	this->glsl4Varbuffers.clear();

#if GL4_MULTIBIND
	// delete varblock binds
	if (this->varblockBindsCount > 0)
	{
		delete[] this->varblockRangeBindBuffers;
		delete[] this->varblockRangeBindOffsets;
		delete[] this->varblockRangeBindSizes;
	}

	// delete varbuffer binds		    
	if (this->varbufferBindsCount > 0)
	{
		delete[] this->varbufferRangeBindBuffers;
		delete[] this->varbufferRangeBindOffsets;
		delete[] this->varbufferRangeBindSizes;
	}	

	// delete texture binds			    
	if (this->textureBindsCount) delete[] this->textureBinds;
	if (this->imageBindsCount) delete[] this->imageBinds;
#endif
}

//------------------------------------------------------------------------------
/**
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

#if GL4_MULTIBIND
	this->varblocksDirty = true;
	this->varbuffersDirty = true;
	this->texturesDirty = true;
	this->imagesDirty = true;
#endif

	// if we support tessellation, then set the patch vertices parameter also
	if (this->supportsTessellation)
	{
        if (GLSL4GlobalProgramState::patchSize != this->patchSize)
        {
            GLSL4GlobalProgramState::patchSize = this->patchSize;
            glPatchParameteri(GL_PATCH_VERTICES, this->patchSize);
        }		
	}

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
                this->error = eastl::string(log, size);
				delete[] log;

				// output false
				return false;
			}
			else
			{
				// set warning flag
                this->warning = eastl::string(log, size);
				delete[] log;
			}
		}
		else
		{
			glValidateProgram(this->programHandle);

			GLint size;
			glGetProgramiv(this->programHandle, GL_INFO_LOG_LENGTH, &size);
			if (size > 0)
			{
				GLchar* log = new GLchar[size];
				glGetProgramInfoLog(this->programHandle, size, NULL, log);
				printf("glValidateProgram produced: %s\n", log);
			}
		}
	}
	else
	{
		// set to default program
		glDeleteProgram(this->programHandle);
		this->programHandle = 0;

		// destroy shaders
		this->DestroyShaders();

        // return false since the program is empty
        return false;
	}

	// destroy shaders
	this->DestroyShaders();

	return true;
}

//------------------------------------------------------------------------------
/**
*/
void
GLSL4EffectProgram::Commit()
{
#if GL4_MULTIBIND
	// signal our variables and varblocks to apply their variables
	unsigned i;
	size_t num = this->glsl4Variables.size();
	for (i = 0; i < num; ++i)
	{
		this->glsl4Variables[i]->Commit();
	}

	num = this->glsl4Varbuffers.size();
	for (i = 0; i < num; ++i)
	{
		this->glsl4Varbuffers[i]->Commit();
	}

	num = this->glsl4Varblocks.size();
	for (i = 0; i < num; ++i)
	{
		this->glsl4Varblocks[i]->Commit();
	}

	if (this->textureBindsCount > 0)
	{
		glBindTextures(0, this->textureBindsCount, this->textureBinds);
		this->texturesDirty = false;
	}
	if (this->imageBindsCount > 0)
	{
		glBindImageTextures(0, this->imageBindsCount, this->imageBinds);
		this->imagesDirty = false;
	}
	if (this->varbufferBindsCount > 0)
		glBindBuffersRange(GL_SHADER_STORAGE_BUFFER, 0, this->varbufferBindsCount, this->varbufferRangeBindBuffers, this->varbufferRangeBindOffsets, this->varbufferRangeBindSizes);
	if (this->varblockBindsCount > 0)
		glBindBuffersRange(GL_UNIFORM_BUFFER, 0, this->varblockBindsCount, this->varblockRangeBindBuffers, this->varblockRangeBindOffsets, this->varblockRangeBindSizes);
#else
	InternalEffectProgram::Commit();
#endif
}

//------------------------------------------------------------------------------
/**
    sort hook for subroutine uniforms
*/
bool
SubroutineBindingCompare(const eastl::pair<GLuint, GLuint>& v1, const eastl::pair<GLuint, GLuint>& v2)
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
GLSL4EffectProgram::SetupSubroutineHelper(GLenum shaderType, GLsizei& numBindings, GLuint** bindingArray, const eastl::map<eastl::string, InternalEffectSubroutine*>& bindings)
{
    eastl::map<eastl::string, InternalEffectSubroutine*>::const_iterator it;

    eastl::vector<eastl::pair<GLuint, GLuint> > intermediateMap;
    unsigned numActiveSubroutines = 0;
    for (it = bindings.begin(); it != bindings.end(); it++)
    {
        eastl::string var = (*it).first;
        InternalEffectSubroutine* imp = (*it).second;

        GLint subroutineIndex = glGetSubroutineIndex(this->programHandle, shaderType, imp->GetName().c_str());
        GLint uniformIndex = glGetSubroutineUniformLocation(this->programHandle, shaderType, var.c_str());

        if (uniformIndex != -1 && subroutineIndex != -1)
        {
            intermediateMap.push_back(eastl::pair<GLuint, GLuint>(uniformIndex, subroutineIndex));
            numActiveSubroutines++;
        }
    }

    GLint numRoutines;
    glGetProgramStageiv(this->programHandle, shaderType, GL_ACTIVE_SUBROUTINE_UNIFORM_LOCATIONS, &numRoutines);
    assert(numActiveSubroutines == numRoutines);

    (*bindingArray) = new GLuint[numActiveSubroutines];
    eastl::sort(intermediateMap.begin(), intermediateMap.end(), SubroutineBindingCompare);
    unsigned i;
    for (i = 0; i < numActiveSubroutines; i++)
    {
        (*bindingArray)[i] = intermediateMap[i].second;
    }

    numBindings = numActiveSubroutines;
}

//------------------------------------------------------------------------------
/**
*/
void
GLSL4EffectProgram::LoadingDone()
{
#if GL4_MULTIBIND
	if (this->varblockBindsCount > 0)
	{
		this->varblockRangeBindBuffers = new GLuint[this->varblockBindsCount];
		this->varblockRangeBindOffsets = new GLint[this->varblockBindsCount];
		this->varblockRangeBindSizes = new GLint[this->varblockBindsCount];
		memset(this->varblockRangeBindBuffers, 0, this->varblockBindsCount * sizeof(GLuint));
		memset(this->varblockRangeBindOffsets, 0, this->varblockBindsCount * sizeof(GLint));
		memset(this->varblockRangeBindSizes, 1, this->varblockBindsCount * sizeof(GLint));
	}	

	if (this->varbufferBindsCount > 0)
	{
		this->varbufferRangeBindBuffers = new GLuint[this->varbufferBindsCount];
		this->varbufferRangeBindOffsets = new GLint[this->varbufferBindsCount];
		this->varbufferRangeBindSizes = new GLint[this->varbufferBindsCount];
		memset(this->varbufferRangeBindBuffers, 0, this->varbufferBindsCount * sizeof(GLuint));
		memset(this->varbufferRangeBindOffsets, 0, this->varbufferBindsCount * sizeof(GLint));
		memset(this->varbufferRangeBindSizes, 1, this->varbufferBindsCount * sizeof(GLint));
	}
	
	if (this->textureBindsCount > 0)
	{
		this->textureBinds = new GLuint[this->textureBindsCount];
		memset(this->textureBinds, 0, this->textureBindsCount * sizeof(GLuint));
	}	

	if (this->imageBindsCount > 0)
	{
		this->imageBinds = new GLuint[this->imageBindsCount];
		memset(this->imageBinds, 0, this->imageBindsCount * sizeof(GLuint));
	}	
#endif
}

//------------------------------------------------------------------------------
/**
*/
void
GLSL4EffectProgram::DestroyShaders()
{
	if (0 != this->shaderBlock.vs)
	{
		GLSL4EffectShader* glsl4Shader = static_cast<GLSL4EffectShader*>(this->shaderBlock.vs);
		glDeleteShader(glsl4Shader->GetShaderHandle());
	}

	if (0 != this->shaderBlock.hs)
	{
		GLSL4EffectShader* glsl4Shader = static_cast<GLSL4EffectShader*>(this->shaderBlock.hs);
		glDeleteShader(glsl4Shader->GetShaderHandle());
	}

	if (0 != this->shaderBlock.ds)
	{
		GLSL4EffectShader* glsl4Shader = static_cast<GLSL4EffectShader*>(this->shaderBlock.ds);
		glDeleteShader(glsl4Shader->GetShaderHandle());
	}

	if (0 != this->shaderBlock.gs)
	{
		GLSL4EffectShader* glsl4Shader = dynamic_cast<GLSL4EffectShader*>(this->shaderBlock.gs);
		glDeleteShader(glsl4Shader->GetShaderHandle());
	}

	if (0 != this->shaderBlock.ps)
	{
		GLSL4EffectShader* glsl4Shader = static_cast<GLSL4EffectShader*>(this->shaderBlock.ps);
		glDeleteShader(glsl4Shader->GetShaderHandle());
	}

	if (0 != this->shaderBlock.cs)
	{
		GLSL4EffectShader* glsl4Shader = static_cast<GLSL4EffectShader*>(this->shaderBlock.cs);
		glDeleteShader(glsl4Shader->GetShaderHandle());
	}
}

} // namespace AnyFX
