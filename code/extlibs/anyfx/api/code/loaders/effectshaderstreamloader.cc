//------------------------------------------------------------------------------
//  effectshaderstreamloader.cc
//  (C) 2013 Gustav Sterbrant
//------------------------------------------------------------------------------
#include "effectshaderstreamloader.h"
#include "effectshader.h"
#include "internal/internaleffectshader.h"
#include "internal/glsl4/glsl4effectshader.h"
#include "effect.h"

namespace AnyFX
{

//------------------------------------------------------------------------------
/**
*/
EffectShaderStreamLoader::EffectShaderStreamLoader()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
EffectShaderStreamLoader::~EffectShaderStreamLoader()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
EffectShader* 
EffectShaderStreamLoader::Load( BinReader* reader, Effect* effect)
{
	InternalEffectShader* internalShader = 0;

	// we should create our implementation back-end first
	if (effect->GetType() == Effect::GLSL)
	{
		if (effect->GetMajor() == 4) internalShader = new GLSL4EffectShader;
	}
	internalShader->effect = effect;

	// get data
	InternalEffectShader::InternalShaderType shaderType = (InternalEffectShader::InternalShaderType)reader->ReadInt();
    eastl::string name = reader->ReadString().c_str();
    eastl::string code = reader->ReadString().c_str();

	internalShader->type = (InternalEffectShader::InternalShaderType)shaderType;
	internalShader->name = name;
	internalShader->sourceCode = code;
	bool success = internalShader->Compile();

    if (internalShader->type == InternalEffectShader::ComputeShader)
    {
        internalShader->localSizes[0] = reader->ReadInt();
        internalShader->localSizes[1] = reader->ReadInt();
        internalShader->localSizes[2] = reader->ReadInt();
    }

	if (success)
	{
		EffectShader* shader = new EffectShader;
		shader->internalShader = internalShader;
		return shader;
	}
	else
	{
		delete internalShader;
		return NULL;
	}	
}
} // namespace AnyFX
