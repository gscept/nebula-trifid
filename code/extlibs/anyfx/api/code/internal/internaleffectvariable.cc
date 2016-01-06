//------------------------------------------------------------------------------
//  internaleffectvariable.cc
//  (C) 2013 Gustav Sterbrant
//------------------------------------------------------------------------------
#include "internaleffectvariable.h"
#include "internaleffectvarblock.h"
#include "internaleffectsampler.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if _MSC_VER
// remove warning for vsnprintf being 'unsafe', pfft
#pragma warning( disable : 4996 )

// remove loss of data conversion from double to float
#pragma warning( disable : 4244 )
#endif

namespace AnyFX
{
unsigned InternalEffectVariable::globalTextureCounter = 0;
//------------------------------------------------------------------------------
/**
*/
InternalEffectVariable::InternalEffectVariable() :
	byteSize(0),
	byteOffset(0),
    sharedByteOffset(NULL),
	currentValue(0),
	active(false),
	isInVarblock(false),
	isDirty(false),
	isArray(false),
	hasDefaultValue(false),
	arraySize(1),
	commitSize(1),
	type(Undefined),
	parentBlock(NULL),
	activeProgram(NULL),
	format(NoFormat),
	access(NoAccess)
{
	this->Retain();
}

//------------------------------------------------------------------------------
/**
*/
InternalEffectVariable::~InternalEffectVariable()
{
	// unless the type is a texture, delete the data buffer
	if (!(this->type >= Sampler1D && this->type <= ImageCubeArray))
	{
		delete [] this->currentValue;
	}
    this->parentBlock = NULL;
    if (0 != this->sharedByteOffset) delete this->sharedByteOffset;
}

//------------------------------------------------------------------------------
/**
	Call this from subclass
*/
void
InternalEffectVariable::Setup(eastl::vector<InternalEffectProgram*> program, const eastl::string& defaultValue)
{
    eastl::string typeString = EffectVariable::TypeToString(this->type);
	this->signature = typeString + ":" + this->name;

	if (this->type >= Sampler1D && this->type <= ImageCubeArray)
	{
		this->MakeTexture();
	}
	else
	{
		this->byteSize = EffectVariable::TypeToByteSize(this->type) * this->arraySize;
		this->currentValue = new char[this->byteSize];
		if (this->hasDefaultValue)
		{
			this->SetupDefaultValue(defaultValue);
		}
        else
        {
            memset(this->currentValue, 0, this->byteSize);
        }
	}
}

//------------------------------------------------------------------------------
/**
*/
void 
InternalEffectVariable::SetupSlave(eastl::vector<InternalEffectProgram*> program, InternalEffectVariable* master)
{
    // empty, override in subclass
}

//------------------------------------------------------------------------------
/**
*/
void 
InternalEffectVariable::MakeTexture()
{
	// override me!
}

//------------------------------------------------------------------------------
/**
*/
void
InternalEffectVariable::Activate(InternalEffectProgram* program)
{
	this->isDirty = true;
	if (this->activeProgram != program)
	{
		this->isDirty = true;
		this->activeProgram = program;
	}
}

//------------------------------------------------------------------------------
/**
*/
void 
InternalEffectVariable::Deactivate()
{
	// override me!
}

//------------------------------------------------------------------------------
/**
*/
void 
InternalEffectVariable::SetupDefaultValue(const eastl::string& string)
{
	unsigned numValues = 0;
	if (string.length() == 0) return;
    eastl::string copy = string;
	char* data = &copy[0];
	char* str = strtok(data, ",");
	while (str)
	{
		switch (this->type)
		{
		case Float:
		case Float2:
		case Float3:
		case Float4:
		case Double:
		case Double2:
		case Double3:
		case Double4:
		case Matrix2x2:
		case Matrix2x3:
		case Matrix2x4:
		case Matrix3x2:
		case Matrix3x3:
		case Matrix3x4:
		case Matrix4x2:
		case Matrix4x3:
		case Matrix4x4:
			{
				float value = (float)atof(str);
				memcpy((void*)(this->currentValue + numValues * sizeof(float)), (void*)&value, sizeof(float));
				break;
			}			
		case Integer:
		case Integer2:
		case Integer3:
		case Integer4:
		case UInteger:
		case UInteger2:
		case UInteger3:
		case UInteger4:
            {
                int value = atoi(str);
                memcpy((void*)(this->currentValue + numValues * sizeof(int)), (void*)&value, sizeof(int));
                break;
            }	
		case Bool:
		case Bool2:
		case Bool3:
		case Bool4:
			{
				int value = atoi(str);
				memcpy((void*)(this->currentValue + numValues * sizeof(bool)), (void*)&value, sizeof(bool));
				break;
			}			
		}
		str = strtok(NULL, ",");
		numValues++;
	}
}


//------------------------------------------------------------------------------
/**
*/
void 
InternalEffectVariable::InitializeDefaultValues()
{
	// override in subclass
}

//------------------------------------------------------------------------------
/**
*/
void 
InternalEffectVariable::Apply()
{
	// override in subclass
}

//------------------------------------------------------------------------------
/**
*/
void 
InternalEffectVariable::Commit()
{
	// override in subclass
}

//------------------------------------------------------------------------------
/**
	Implement in subclass if our back-end supports setting single variables
*/
void
InternalEffectVariable::SetFloat(float f)
{
	if (!this->isInVarblock && memcmp(this->currentValue, &f, sizeof(float)) != 0)
	{
		memcpy((void*)this->currentValue, (void*)&f, sizeof(float));
		this->isDirty = true;
	}		
}

//------------------------------------------------------------------------------
/**
	Implement in subclass if our back-end supports setting single variables
*/
void
InternalEffectVariable::SetFloat2(const float* vec)
{
	if (!this->isInVarblock && memcmp(this->currentValue, vec, 2 * sizeof(float)) != 0)
	{
		memcpy((void*)this->currentValue, (void*)vec, 2 * sizeof(float));
		this->isDirty = true;
	}	
}

//------------------------------------------------------------------------------
/**
*/
void
InternalEffectVariable::SetFloat3(const float* vec)
{
	if (!this->isInVarblock && memcmp(this->currentValue, vec, 3 * sizeof(float)) != 0)
	{
		memcpy((void*)this->currentValue, (void*)vec, 3 * sizeof(float));
		this->isDirty = true;
	}
}

//------------------------------------------------------------------------------
/**
	Implement in subclass if our back-end supports setting single variables
*/
void
InternalEffectVariable::SetFloat4(const float* vec)
{
	if (!this->isInVarblock && memcmp(this->currentValue, vec, 4 * sizeof(float)) != 0)
	{
		memcpy((void*)this->currentValue, (void*)vec, 4 * sizeof(float));
		this->isDirty = true;
	}
}

//------------------------------------------------------------------------------
/**
*/
void
InternalEffectVariable::SetFloatArray(const float* f, size_t count)
{
	assert(this->isArray);
	if (!this->isInVarblock && memcmp(this->currentValue, f, count * sizeof(float)) != 0)
	{
		memcpy((void*)this->currentValue, (void*)f, count * sizeof(float));
		this->commitSize = count;
		this->isDirty = true;
	}	
}

//------------------------------------------------------------------------------
/**
*/
void
InternalEffectVariable::SetFloat2Array(const float* vec, size_t count)
{
	assert(this->isArray);
	if (!this->isInVarblock && memcmp(this->currentValue, vec, count * 2 * sizeof(float)) != 0)
	{
		memcpy((void*)this->currentValue, (void*)vec, count * 2 * sizeof(float));
		this->commitSize = count;
		this->isDirty = true;
	}
}

//------------------------------------------------------------------------------
/**
*/
void
InternalEffectVariable::SetFloat3Array(const float* vec, size_t count)
{
	assert(this->isArray);
	if (!this->isInVarblock && memcmp(this->currentValue, vec, count * 3 * sizeof(float)) != 0)
	{
		memcpy((void*)this->currentValue, (void*)vec, count * 3 * sizeof(float));
		this->commitSize = count;
		this->isDirty = true;
	}	
}

//------------------------------------------------------------------------------
/**
*/
void
InternalEffectVariable::SetFloat4Array(const float* vec, size_t count)
{
	assert(this->isArray);
	if (!this->isInVarblock && memcmp(this->currentValue, vec, count * 4 * sizeof(float)) != 0)
	{
		memcpy((void*)this->currentValue, (void*)vec, count * 4 * sizeof(float));
		this->commitSize = count;
		this->isDirty = true;
	}
}

//------------------------------------------------------------------------------
/**
	Implement in subclass if our back-end supports setting single variables
*/
void
InternalEffectVariable::SetInt(int i)
{
	if (!this->isInVarblock && memcmp(this->currentValue, &i, sizeof(int)) != 0)
	{
		memcpy((void*)this->currentValue, (void*)&i, sizeof(int));
		this->isDirty = true;
	}
}

//------------------------------------------------------------------------------
/**
	Implement in subclass if our back-end supports setting single variables
*/
void
InternalEffectVariable::SetInt2(const int* vec)
{
	if (!this->isInVarblock && memcmp(this->currentValue, vec, 2 * sizeof(int)) != 0)
	{
		memcpy((void*)this->currentValue, (void*)vec, 2 * sizeof(int));
		this->isDirty = true;
	}
}

//------------------------------------------------------------------------------
/**
*/
void
InternalEffectVariable::SetInt3(const int* vec)
{
	if (!this->isInVarblock && memcmp(this->currentValue, vec, 3 * sizeof(int)) != 0)
	{
		memcpy((void*)this->currentValue, (void*)vec, 3 * sizeof(int));
		this->isDirty = true;
	}
}

//------------------------------------------------------------------------------
/**
	Implement in subclass if our back-end supports setting single variables
*/
void
InternalEffectVariable::SetInt4(const int* vec)
{
	if (this->isInVarblock)
	if (!this->isInVarblock && memcmp(this->currentValue, vec, 4 * sizeof(int)) != 0)
	{
		memcpy((void*)this->currentValue, (void*)vec, 4 * sizeof(int));
		this->isDirty = true;
	}
}

//------------------------------------------------------------------------------
/**
*/
void
InternalEffectVariable::SetIntArray(const int* i, size_t count)
{
	assert(this->isArray);
	if (!this->isInVarblock && memcmp(this->currentValue, i, count * sizeof(int)) != 0)
	{
		memcpy((void*)this->currentValue, (void*)i, count * sizeof(int));
		this->commitSize = count;
		this->isDirty = true;
	}
}

//------------------------------------------------------------------------------
/**
*/
void
InternalEffectVariable::SetInt2Array(const int* vec, size_t count)
{
	assert(this->isArray);
	if (!this->isInVarblock && memcmp(this->currentValue, vec, count * 2 * sizeof(int)) != 0)
	{
		memcpy((void*)this->currentValue, (void*)vec, count * 2 * sizeof(int));
		this->commitSize = count;
		this->isDirty = true;
	}
}

//------------------------------------------------------------------------------
/**
*/
void
InternalEffectVariable::SetInt3Array(const int* vec, size_t count)
{
	assert(this->isArray);
	if (!this->isInVarblock && memcmp(this->currentValue, vec, count * 3 * sizeof(int)) != 0)
	{
		memcpy((void*)this->currentValue, (void*)vec, count * 3 * sizeof(int));
		this->commitSize = count;
		this->isDirty = true;
	}
}

//------------------------------------------------------------------------------
/**
*/
void
InternalEffectVariable::SetInt4Array(const int* vec, size_t count)
{
	assert(this->isArray);
	if (!this->isInVarblock && memcmp(this->currentValue, vec, count * 4 * sizeof(int)) != 0)
	{
		memcpy((void*)this->currentValue, (void*)vec, count * 4 * sizeof(int));
		this->commitSize = count;
		this->isDirty = true;
	}
}

//------------------------------------------------------------------------------
/**
	Implement in subclass if our back-end supports setting single variables
*/
void
InternalEffectVariable::SetBool(bool b)
{
	if (!this->isInVarblock && memcmp(this->currentValue, &b, sizeof(bool)) != 0)
	{
		memcpy((void*)this->currentValue, (void*)&b, sizeof(bool));
		this->isDirty = true;
	}
}

//------------------------------------------------------------------------------
/**
	Implement in subclass if our back-end supports setting single variables
*/
void
InternalEffectVariable::SetBool2(const bool* vec)
{
	if (!this->isInVarblock && memcmp(this->currentValue, vec, 2 * sizeof(bool)) != 0)
	{
		memcpy((void*)this->currentValue, (void*)vec, 2 * sizeof(bool));
		this->isDirty = true;
	}
}

//------------------------------------------------------------------------------
/**
*/
void
InternalEffectVariable::SetBool3(const bool* vec)
{
	if (!this->isInVarblock && memcmp(this->currentValue, vec, 3 * sizeof(bool)) != 0)
	{
		memcpy((void*)this->currentValue, (void*)vec, 3 * sizeof(bool));
		this->isDirty = true;
	}
}

//------------------------------------------------------------------------------
/**
	Implement in subclass if our back-end supports setting single variables
*/
void
InternalEffectVariable::SetBool4(const bool* vec)
{
	if (!this->isInVarblock && memcmp(this->currentValue, vec, 4 * sizeof(bool)) != 0)
	{
		memcpy((void*)this->currentValue, (void*)vec, 4 * sizeof(bool));
		this->isDirty = true;
	}
}

//------------------------------------------------------------------------------
/**
*/
void
InternalEffectVariable::SetBoolArray(const bool* b, size_t count)
{
	assert(this->isArray);
	if (!this->isInVarblock && memcmp(this->currentValue, b, count * sizeof(bool)) != 0)
	{
		memcpy((void*)this->currentValue, (void*)b, count * sizeof(bool));
		this->commitSize = count;
		this->isDirty = true;
	}
}

//------------------------------------------------------------------------------
/**
*/
void
InternalEffectVariable::SetBool2Array(const bool* vec, size_t count)
{
	assert(this->isArray);
	if (!this->isInVarblock && memcmp(this->currentValue, vec, count * 2 * sizeof(bool)) != 0)
	{
		memcpy((void*)this->currentValue, (void*)vec, count * 2 * sizeof(bool));
		this->commitSize = count;
		this->isDirty = true;
	}
}

//------------------------------------------------------------------------------
/**
*/
void
InternalEffectVariable::SetBool3Array(const bool* vec, size_t count)
{
	assert(this->isArray);
	if (!this->isInVarblock && memcmp(this->currentValue, vec, count * 3 * sizeof(bool)) != 0)
	{
		memcpy((void*)this->currentValue, (void*)vec, count * 3 * sizeof(bool));
		this->commitSize = count;
		this->isDirty = true;
	}
}

//------------------------------------------------------------------------------
/**
*/
void
InternalEffectVariable::SetBool4Array(const bool* vec, size_t count)
{
	assert(this->isArray);
	if (!this->isInVarblock && memcmp(this->currentValue, vec, count * 4 * sizeof(bool)) != 0)
	{
		memcpy((void*)this->currentValue, (void*)vec, count * 4 * sizeof(bool));
		this->commitSize = count;
		this->isDirty = true;
	}
}

//------------------------------------------------------------------------------
/**
	Implement in subclass if our back-end supports setting single variables
*/
void
InternalEffectVariable::SetMatrix(const float* mat)
{
	if (!this->isInVarblock && memcmp(this->currentValue, mat, 16 * sizeof(float)) != 0)
	{
		memcpy((void*)this->currentValue, (void*)mat, 16 * sizeof(float));
		this->isDirty = true;
	}
}

//------------------------------------------------------------------------------
/**
*/
void
InternalEffectVariable::SetMatrixArray(const float* mat, size_t count)
{
	if (!this->isInVarblock && memcmp(this->currentValue, mat, count * 16 * sizeof(float)) != 0)
	{
		memcpy((void*)this->currentValue, (void*)mat, count * 16 * sizeof(float));
		this->commitSize = count;
		this->isDirty = true;
	}
}

//------------------------------------------------------------------------------
/**
	Setting textures is not provided by any block-wise manner, 
	so here we must have an implementation-specific solution.

	We do this by providing 

	Simply point this current value buffer to handle
*/
void
InternalEffectVariable::SetTexture(void* handle)
{
	if (this->currentValue != handle)
	{
		this->currentValue = (char*)handle;
		this->isDirty = true;
	}	
}

//------------------------------------------------------------------------------
/**
*/
void 
InternalEffectVariable::SetFloatIndexed(float f, unsigned index)
{
    assert(this->isArray);
}

//------------------------------------------------------------------------------
/**
*/
void 
InternalEffectVariable::SetFloat2Indexed(const float* vec, unsigned index)
{
    assert(this->isArray);
}

//------------------------------------------------------------------------------
/**
*/
void 
InternalEffectVariable::SetFloat3Indexed(const float* vec, unsigned index)
{
    assert(this->isArray);
}

//------------------------------------------------------------------------------
/**
*/
void 
InternalEffectVariable::SetFloat4Indexed(const float* vec, unsigned index)
{
    assert(this->isArray);
}  

//------------------------------------------------------------------------------
/**
*/
void 
InternalEffectVariable::SetIntIndexed(int i, unsigned index)
{
    assert(this->isArray);
}

//------------------------------------------------------------------------------
/**
*/
void 
InternalEffectVariable::SetInt2Indexed(const int* vec, unsigned index)
{
    assert(this->isArray);
}

//------------------------------------------------------------------------------
/**
*/
void 
InternalEffectVariable::SetInt3Indexed(const int* vec, unsigned index)
{
    assert(this->isArray);
}

//------------------------------------------------------------------------------
/**
*/
void 
InternalEffectVariable::SetInt4Indexed(const int* vec, unsigned index)
{
    assert(this->isArray);
}

//------------------------------------------------------------------------------
/**
*/
void 
InternalEffectVariable::SetBoolIndexed(bool b, unsigned index)
{
    assert(this->isArray);
}

//------------------------------------------------------------------------------
/**
*/
void 
InternalEffectVariable::SetBool2Indexed(const bool* vec, unsigned index)
{
    assert(this->isArray);
}

//------------------------------------------------------------------------------
/**
*/
void 
InternalEffectVariable::SetBool3Indexed(const bool* vec, unsigned index)
{
    assert(this->isArray);
}

//------------------------------------------------------------------------------
/**
*/
void 
InternalEffectVariable::SetBool4Indexed(const bool* vec, unsigned index)
{
    assert(this->isArray);
}

//------------------------------------------------------------------------------
/**
*/
void 
InternalEffectVariable::SetMatrixIndexed(const float* mat, unsigned index)
{
    assert(this->isArray);
}

//------------------------------------------------------------------------------
/**
*/
void 
InternalEffectVariable::SetTextureIndexed(void* handle, unsigned index)
{
    assert(this->isArray);
}

} // namespace AnyFX
