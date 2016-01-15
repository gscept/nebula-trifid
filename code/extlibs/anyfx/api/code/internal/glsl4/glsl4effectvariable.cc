//------------------------------------------------------------------------------
//  glsl4effectvariable.cc
//  (C) 2013 Gustav Sterbrant
//------------------------------------------------------------------------------
#include "glsl4effectvariable.h"
#include "glsl4effectprogram.h"
#include <assert.h>
#include "internal/internaleffectvarblock.h"

#define a_max(x, y) (x > y ? x : y)
#if __ANYFX_TRANSPOSE_MATRIX__
	#define __MATRIX_TRANSPOSE__ GL_TRUE
#else
	#define __MATRIX_TRANSPOSE__ GL_FALSE
#endif

static const GLenum opengl4AccessTable[] = 
{
	GL_READ_ONLY,
	GL_WRITE_ONLY,
	GL_READ_WRITE
};

static const GLenum opengl4ImageFormat[] =
{
	GL_RGBA32F,
	GL_RGBA16F,
	GL_RG32F,
	GL_RG16F,
	GL_R11F_G11F_B10F,
	GL_R32F,
	GL_R16F,
	GL_RGBA16,
	GL_RGB10_A2,
    GL_RGBA8,
	GL_RG16,
	GL_RG8,
    GL_R16,
	GL_R8,
	GL_RGBA16_SNORM,
	GL_RGBA8_SNORM,
	GL_RG16_SNORM,
	GL_RG8_SNORM,
	GL_R16_SNORM,
	GL_R8_SNORM,
	GL_RGBA32I,
	GL_RGBA16I,
	GL_RGBA8I,
	GL_RG32I,
	GL_RG16I,
	GL_RG8I,
	GL_R32I,
	GL_R16I,
	GL_R8I,
	GL_RGBA32UI,
	GL_RGBA16UI,
    GL_RGB10_A2UI,
	GL_RGBA8UI,
	GL_RG32UI,
	GL_RG16UI,
	GL_RG8UI,
	GL_R32UI,
	GL_R16UI,
	GL_R8UI
};

namespace AnyFX
{

//------------------------------------------------------------------------------
/**
*/
GLSL4EffectVariable::GLSL4EffectVariable() :
	uniformLocation(-1),
	activeProgramHandle(0),
	glAccessMode(GL_NONE),
	glImageFormat(GL_NONE),
	textureUnit(GL_TEXTURE0),
	textureType(GL_TEXTURE_2D)
{
	this->uniformProgramMap.clear();
}

//------------------------------------------------------------------------------
/**
*/
GLSL4EffectVariable::~GLSL4EffectVariable()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
void 
GLSL4EffectVariable::Setup(eastl::vector<InternalEffectProgram*> programs, const eastl::string& defaultValue)
{
	InternalEffectVariable::Setup(programs, defaultValue);

	if (!this->isInVarblock)
	{
		unsigned i;
		for (i = 0; i < programs.size(); i++)
		{
            GLSL4EffectProgram* opengl4Program = dynamic_cast<GLSL4EffectProgram*>(programs[i]);
            assert(0 != opengl4Program);
#if GL4_MULTIBIND
			opengl4Program->glsl4Variables.push_back(this);
			if (this->type >= Sampler1D && this->type <= SamplerCubeArray) opengl4Program->textureBindsCount = a_max(this->textureUnit + 1, opengl4Program->textureBindsCount);
			if (this->type >= Image1D && this->type <= ImageCubeArray) opengl4Program->imageBindsCount = a_max(this->textureUnit + 1, opengl4Program->imageBindsCount);
#endif

            if (this->uniformProgramMap.find(opengl4Program->programHandle) == this->uniformProgramMap.end())
            {
			    GLint location = glGetUniformLocation(opengl4Program->programHandle, this->name.c_str());
			    this->uniformProgramMap[opengl4Program->programHandle] = location;

                if (location != -1)
                {
                    this->active = true;
                }
            }
		}
	}
	else
	{
		// unfortunately, we cannot know if a variable inside a variable block is used...
		this->active = true;
	}
}

//------------------------------------------------------------------------------
/**
*/
void 
GLSL4EffectVariable::SetupSlave(eastl::vector<InternalEffectProgram*> programs, InternalEffectVariable* master)
{
    InternalEffectVariable::SetupSlave(programs, master);

    if (!this->isInVarblock)
    {
        unsigned i;
        for (i = 0; i < programs.size(); i++)
        {
            GLSL4EffectProgram* opengl4Program = dynamic_cast<GLSL4EffectProgram*>(programs[i]);
            assert(0 != opengl4Program);
#if GL4_MULTIBIND
			opengl4Program->glsl4Variables.push_back(this);
			if (this->type >= Sampler1D && this->type <= SamplerCubeArray) opengl4Program->textureBindsCount = a_max(this->textureUnit, opengl4Program->textureBindsCount);
			if (this->type >= Image1D && this->type <= ImageCubeArray) opengl4Program->imageBindsCount = a_max(this->textureUnit, opengl4Program->imageBindsCount);
#endif

            if (this->uniformProgramMap.find(opengl4Program->programHandle) == this->uniformProgramMap.end())
            {
                GLint location = glGetUniformLocation(opengl4Program->programHandle, this->name.c_str());
                this->uniformProgramMap[opengl4Program->programHandle] = location;
            }            
        }
    }
}

//------------------------------------------------------------------------------
/**
*/
void 
GLSL4EffectVariable::MakeTexture()
{
	if (this->type >= Sampler1D && this->type <= SamplerCubeArray) this->textureUnit = InternalEffectVariable::globalTextureCounter++;
	if (this->type >= Image1D && this->type <= ImageCubeArray) this->textureUnit = InternalEffectVariable::globalImageCounter++;

	switch (this->type)
	{
	case Image1D:
		this->glAccessMode = opengl4AccessTable[this->access];
		this->glImageFormat = opengl4ImageFormat[this->format];
	case Sampler1D:
		this->textureType = GL_TEXTURE_1D;
		break;
	case Image1DArray:
		this->glAccessMode = opengl4AccessTable[this->access];
		this->glImageFormat = opengl4ImageFormat[this->format];
	case Sampler1DArray:
		this->textureType = GL_TEXTURE_1D_ARRAY;
		break;
	case Image2D:
		this->glAccessMode = opengl4AccessTable[this->access];
		this->glImageFormat = opengl4ImageFormat[this->format];
	case Sampler2D:
		this->textureType = GL_TEXTURE_2D;
		break;
	case Image2DArray:
		this->glAccessMode = opengl4AccessTable[this->access];
		this->glImageFormat = opengl4ImageFormat[this->format];
	case Sampler2DArray:
		this->textureType = GL_TEXTURE_2D_ARRAY;
		break;
	case Image2DMS:
		this->glAccessMode = opengl4AccessTable[this->access];
		this->glImageFormat = opengl4ImageFormat[this->format];
	case Sampler2DMS:
		this->textureType = GL_TEXTURE_2D_MULTISAMPLE;
		break;
	case Image2DMSArray:
		this->glAccessMode = opengl4AccessTable[this->access];
		this->glImageFormat = opengl4ImageFormat[this->format];
	case Sampler2DMSArray:
		this->textureType = GL_TEXTURE_2D_MULTISAMPLE_ARRAY;
		break;
	case Image3D:
		this->glAccessMode = opengl4AccessTable[this->access];
		this->glImageFormat = opengl4ImageFormat[this->format];
	case Sampler3D:
		this->textureType = GL_TEXTURE_3D;
		break;
	case ImageCube:
		this->glAccessMode = opengl4AccessTable[this->access];
		this->glImageFormat = opengl4ImageFormat[this->format];
	case SamplerCube:
		this->textureType = GL_TEXTURE_CUBE_MAP;
		break;
	case ImageCubeArray:
		this->glAccessMode = opengl4AccessTable[this->access];
		this->glImageFormat = opengl4ImageFormat[this->format];
	case SamplerCubeArray:
		this->textureType = GL_TEXTURE_CUBE_MAP_ARRAY;
		break;
	}
}

//------------------------------------------------------------------------------
/**
*/
void
GLSL4EffectVariable::Activate(InternalEffectProgram* program)
{
	InternalEffectVariable::Activate(program);
	GLSL4EffectProgram* opengl4Program = static_cast<GLSL4EffectProgram*>(program);
	if (!isInVarblock && this->active)
	{
		assert(this->uniformProgramMap.find(opengl4Program->programHandle) != this->uniformProgramMap.end());
		this->uniformLocation = this->uniformProgramMap[opengl4Program->programHandle];
		this->activeProgramHandle = opengl4Program->programHandle;	
		this->activeProgram = opengl4Program;
	}	
	else
	{
		this->uniformLocation = -1;
        this->byteOffset = this->blockOffsets[opengl4Program->programHandle];
	}
}

//------------------------------------------------------------------------------
/**
*/
void 
GLSL4EffectVariable::Apply()
{
	if (this->type >= Sampler1D && this->type <= ImageCubeArray && this->uniformLocation != -1 && this->isDirty)
	{
		// first bind variable name to texture unit
		glUniform1i(this->uniformLocation, this->textureUnit);
	}	
}

//------------------------------------------------------------------------------
/**
*/
void 
GLSL4EffectVariable::Commit()
{
    // never apply subroutine variables
    if (this->isSubroutine) return;
	if (!this->isInVarblock && this->uniformLocation != -1 && this->isDirty)
	{
		switch (this->type)
		{
		case Float:
		case Double:
			glUniform1fv(this->uniformLocation, this->commitSize, (GLfloat*)this->currentValue);
			break;
		case Float2:
		case Double2:
			glUniform2fv(this->uniformLocation, this->commitSize, (GLfloat*)this->currentValue);
			break;
		case Float3:
		case Double3:
			glUniform3fv(this->uniformLocation, this->commitSize, (GLfloat*)this->currentValue);
			break;
		case Float4:
		case Double4:
			glUniform4fv(this->uniformLocation, this->commitSize, (GLfloat*)this->currentValue);
			break;
		case Integer:
		case Short:
		case Bool:
			glUniform1iv(this->uniformLocation, this->commitSize, (GLint*)this->currentValue);
			break;
		case Integer2:
		case Short2:
		case Bool2:
			glUniform2iv(this->uniformLocation, this->commitSize, (GLint*)this->currentValue);
			break;
		case Integer3:
		case Short3:
		case Bool3:
			glUniform3iv(this->uniformLocation, this->commitSize, (GLint*)this->currentValue);
			break;
		case Integer4:
		case Short4:
		case Bool4:
			glUniform4iv(this->uniformLocation, this->commitSize, (GLint*)this->currentValue);
			break;
		case UInteger:
			glUniform1uiv(this->uniformLocation, this->commitSize, (GLuint*)this->currentValue);
			break;
		case UInteger2:
			glUniform2uiv(this->uniformLocation, this->commitSize, (GLuint*)this->currentValue);
			break;
		case UInteger3:
			glUniform3uiv(this->uniformLocation, this->commitSize, (GLuint*)this->currentValue);
			break;
		case UInteger4:
			glUniform4uiv(this->uniformLocation, this->commitSize, (GLuint*)this->currentValue);
			break;
		case Matrix2x2:
			glUniformMatrix2fv(this->uniformLocation, this->commitSize, __MATRIX_TRANSPOSE__, (GLfloat*)this->currentValue);
			break;
		case Matrix2x3:
			glUniformMatrix2x3fv(this->uniformLocation, this->commitSize, __MATRIX_TRANSPOSE__, (GLfloat*)this->currentValue);
			break;
		case Matrix2x4:
			glUniformMatrix2x4fv(this->uniformLocation, this->commitSize, __MATRIX_TRANSPOSE__, (GLfloat*)this->currentValue);
			break;
		case Matrix3x2:
			glUniformMatrix3x2fv(this->uniformLocation, this->commitSize, __MATRIX_TRANSPOSE__, (GLfloat*)this->currentValue);
			break;
		case Matrix3x3:
			glUniformMatrix3fv(this->uniformLocation, this->commitSize, __MATRIX_TRANSPOSE__, (GLfloat*)this->currentValue);
			break;
		case Matrix3x4:
			glUniformMatrix3x4fv(this->uniformLocation, this->commitSize, __MATRIX_TRANSPOSE__, (GLfloat*)this->currentValue);
			break;
		case Matrix4x2:
			glUniformMatrix4x2fv(this->uniformLocation, this->commitSize, __MATRIX_TRANSPOSE__, (GLfloat*)this->currentValue);
			break;
		case Matrix4x3:
			glUniformMatrix4x3fv(this->uniformLocation, this->commitSize, __MATRIX_TRANSPOSE__, (GLfloat*)this->currentValue);
			break;
		case Matrix4x4:
			glUniformMatrix4fv(this->uniformLocation, this->commitSize, __MATRIX_TRANSPOSE__, (GLfloat*)this->currentValue);
			break;
		case Sampler1D:
		case Sampler1DArray:
		case Sampler2D:
		case Sampler2DArray:
		case Sampler2DMS:
		case Sampler2DMSArray:
		case Sampler3D:
		case SamplerCube:
		case SamplerCubeArray:
			{
                // unpack data
                EffectVariable::OpenGLTextureBinding* obj = (EffectVariable::OpenGLTextureBinding*)this->currentValue;
				if (obj)
				{
#if GL4_MULTIBIND
					if (this->activeProgram->textureBinds[this->textureUnit] != obj->bound.handle)
					{
						this->activeProgram->textureBinds[this->textureUnit] = obj->bound.handle;
						this->activeProgram->texturesDirty = true;
					}
				}
				else
				{
					this->activeProgram->textureBinds[this->textureUnit] = 0;
					this->activeProgram->texturesDirty = true;
				}
#else
                    if (this->bindless)
                    {
                        glUniformHandleui64ARB(this->uniformLocation, obj->notbound.handle);
                    }
                    else
                    {
                        // now select this texture unit to be the active texture
                        glActiveTexture(GL_TEXTURE0 + this->textureUnit);

                        // only set obj if the current value is not null
                        if (obj && obj->bound.textureType == this->textureType)
                        {
                            // now bind the texture to this slot
                            glBindTexture(obj->bound.textureType, obj->bound.handle);
                        }
                        else
                        {
                            // otherwise, bind texture to 0
                            glBindTexture(this->textureType, 0);
                        }
                    }
                }
                else
                {
                    glActiveTexture(GL_TEXTURE0 + this->textureUnit);
                    glBindTexture(GL_TEXTURE_2D, 0);
                }
#endif

				break;
			}
		case Image1D:
		case Image2D:
		case Image2DMS:
            {
                // unpack data
                EffectVariable::OpenGLTextureBinding* obj = (EffectVariable::OpenGLTextureBinding*)this->currentValue;
				
#if GL4_MULTIBIND
				if (obj)
				{
					if (this->activeProgram->imageBinds[this->textureUnit] != obj->bound.handle)
					{
						this->activeProgram->imageBinds[this->textureUnit] = obj->bound.handle;
						this->activeProgram->imagesDirty = true;
					}
				}
				else
				{
					this->activeProgram->imageBinds[this->textureUnit] = 0;
					this->activeProgram->imagesDirty = true;
				}
#else
                if (obj && obj->bound.textureType == this->textureType)
                {
                    // bind the texture to the image unit, this is a bit sensitive since if the texture object doesn't match the image format, the GL will output an error.
					glBindImageTexture(this->textureUnit, obj->bound.handle, 0, GL_FALSE, 0, this->glAccessMode, this->glImageFormat);
                }
                else
                {
                    glBindImageTexture(this->textureUnit, 0, 0, GL_FALSE, 0, this->glAccessMode, this->glImageFormat);
                }
#endif
				
                break;
            }
        case Image1DArray:
        case Image2DArray:
        case Image2DMSArray:
		case Image3D:
		case ImageCube:
		case ImageCubeArray:
			{
				// unpack data
				EffectVariable::OpenGLTextureBinding* obj = (EffectVariable::OpenGLTextureBinding*)this->currentValue;

                if (obj && obj->bound.textureType == this->textureType)
                {
                    // now select this image unit to be the active image (as apparent, this is not very nice since the format and read/write is hard coded)
                    glBindImageTexture(this->textureUnit, obj->bound.handle, 0, GL_TRUE, 0, this->glAccessMode, this->glImageFormat);
                }
                else
                {
					glBindImageTexture(this->textureUnit, 0, 0, GL_TRUE, 0, this->glAccessMode, this->glImageFormat);
                }
				
				break;
			}
		}

		// uncheck dirty flag
		this->isDirty = false;
	}
}

} // namespace AnyFX
