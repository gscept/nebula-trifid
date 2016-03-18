//------------------------------------------------------------------------------
// vkstreamshaderloader.cc
// (C) 2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "vkstreamshaderloader.h"
#include "vkshader.h"
#include "coregraphics/constantbuffer.h"
#include "effectfactory.h"
#include "lowlevel/vk/vkvariable.h"
#include "lowlevel/vk/vkvarblock.h"
#include "lowlevel/vk/vkvarbuffer.h"

using namespace CoreGraphics;
using namespace IO;
namespace Vulkan
{

__ImplementClass(Vulkan::VkStreamShaderLoader, 'VKSL', Resources::StreamResourceLoader);


//------------------------------------------------------------------------------
/**
	TODO: In Vulkan, we might be able to thread the shader creation process
*/
bool
VkStreamShaderLoader::CanLoadAsync() const
{
	return false;
}

//------------------------------------------------------------------------------
/**
*/
bool
VkStreamShaderLoader::SetupResourceFromStream(const Ptr<IO::Stream>& stream)
{
	n_assert(stream.isvalid());
	n_assert(stream->CanBeMapped());
	n_assert(this->resource->IsA(VkShader::RTTI));
	const Ptr<VkShader>& res = this->resource.downcast<VkShader>();
	n_assert(!res->IsLoaded());

	// map stream to memory
	stream->SetAccessMode(Stream::ReadAccess);
	if (stream->Open())
	{
		void* srcData = stream->Map();
		uint srcDataSize = stream->GetSize();

		// load effect from memory
		AnyFX::ShaderEffect* effect = AnyFX::EffectFactory::Instance()->CreateShaderEffectFromMemory(srcData, srcDataSize);

		// catch any potential GL error coming from AnyFX
		if (!effect)
		{
			n_error("VkStreamShaderLoader::SetupResourceFromStream(): failed to load shader '%s'!",
				res->GetResourceId().Value());
			return false;
		}

		res->vkEffect = effect;
		res->shaderName = res->GetResourceId().AsString();
		res->shaderIdentifierCode = ShaderIdentifier::FromName(res->shaderName.AsString());

		// setup shader variations
		const eastl::vector<AnyFX::ProgramBase*>& programs = effect->GetPrograms();
		for (uint i = 0; i < programs.size(); i++)
		{
			// a shader variation in Nebula is equal to a program object in AnyFX
			Ptr<ShaderVariation> variation = ShaderVariation::Create();

			// get program object from shader subsystem
			AnyFX::VkProgram* program = static_cast<AnyFX::VkProgram*>(programs[i]);

			// program is only valid if either vertex shader (for graphics) or compute shader (for computes)
			if (program->shaderBlock.vs != NULL || program->shaderBlock.cs != NULL)
			{
				variation->Setup(program, effect);
				res->variations.Add(variation->GetFeatureMask(), variation);
			}
		}

		// make sure that the shader has one variation selected
		if (!res->variations.IsEmpty()) res->activeVariation = res->variations.ValueAtIndex(0);

		// load uniforms
		const eastl::vector<AnyFX::VariableBase*>& variables = effect->GetVariables();
		for (uint i = 0; i < variables.size(); i++)
		{
			// get AnyFX variable
			AnyFX::VkVariable* effectVar = static_cast<AnyFX::VkVariable*>(variables[i]);

			// create new variable
			Ptr<ShaderVariable> var = ShaderVariable::Create();

			// setup variable from AnyFX variable
			var->Setup(effectVar);
			res->variables.Append(var);
			res->variablesByName.Add(effectVar->name.c_str(), var);
		}

		// load shader storage buffer variables
		const eastl::vector<AnyFX::VarblockBase*>& varblocks = effect->GetVarblocks();
		for (uint i = 0; i < varblocks.size(); i++)
		{
			// get AnyFX variable
			AnyFX::VkVarblock* block = static_cast<AnyFX::VkVarblock*>(varblocks[i]);
			if (block->variables.empty()) continue;

			// create new variable
			Ptr<ShaderVariable> var = ShaderVariable::Create();

			// setup variable from AnyFX varblock
			var->Setup(block);
			res->variables.Append(var);
			res->variablesByName.Add(block->name.c_str(), var);
		}

		// load uniform block variables
		const eastl::vector<AnyFX::VarbufferBase*>& varbuffers = effect->GetVarbuffers();
		for (uint i = 0; i < varbuffers.size(); i++)
		{
			// get varblock
			AnyFX::VkVarbuffer* buffer = static_cast<AnyFX::VkVarbuffer*>(varbuffers[i]);

			// create a new variable
			Ptr<ShaderVariable> var = ShaderVariable::Create();

			// setup variable from AnyFX varbuffer
			var->Setup(buffer);
			res->variables.Append(var);
			res->variablesByName.Add(buffer->name.c_str(), var);
		}

		// setup variables belonging to the 'default' variable block, which is where all global variables end up
		if (effect->HasVarblock("GlobalBlock"))
		{
			// get global block
			AnyFX::VkVarblock* effectBlock = static_cast<AnyFX::VkVarblock*>(effect->GetVarblock("GlobalBlock"));
			
			// get a program, they should all have the same uniform offsets
			AnyFX::VkProgram* program = static_cast<AnyFX::VkProgram*>(programs.front());
			const eastl::vector<AnyFX::VariableBase*>& vars = effectBlock->variables;
			if (!program->variableBlockOffsets.empty())
			{
				// create constant buffer, make it sync because we might have > 1 updates per frame
				res->globalBlockBuffer = ConstantBuffer::Create();
				res->globalBlockBuffer->SetSize(effectBlock->byteSize);
				res->globalBlockBuffer->SetSync(true);
				res->globalBlockBuffer->Setup(1);

				res->globalBlockBuffer->BeginUpdateSync();
				for (unsigned j = 0; j < vars.size(); j++)
				{
					// bind this variable to a shader global buffer, if this variable is instanced, it can either point to its
					// own buffer, or point directly to this block (if variable instance is not bound to a buffer)
					const Ptr<ShaderVariable>& var = res->variablesByName[vars[j]->name.c_str()];
					var->BindToUniformBuffer(res->globalBlockBuffer, program->variableBlockOffsets[vars[j]->name], vars[j]->byteSize, (int8_t*)vars[j]->currentValue);
				}
				res->globalBlockBuffer->EndUpdateSync();
				res->globalBlockBufferVar = res->variablesByName["GlobalBlock"];
			}
		}

		return true;
	}
	return false;
}

} // namespace Vulkan