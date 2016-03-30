//------------------------------------------------------------------------------
// vkshaderinstance.cc
// (C) 2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "vkshaderinstance.h"
#include "coregraphics/constantbuffer.h"
#include "coregraphics/shader.h"

namespace Vulkan
{

__ImplementClass(Vulkan::VkShaderInstance, 'VKSN', Base::ShaderInstanceBase);
//------------------------------------------------------------------------------
/**
*/
VkShaderInstance::VkShaderInstance()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
VkShaderInstance::~VkShaderInstance()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
void
VkShaderInstance::Setup(const Ptr<CoreGraphics::Shader>& origShader)
{
	n_assert(origShader.isvalid());
	const Ptr<VkShader>& vkShader = origShader.upcast<VkShader>();

	// call parent class
	ShaderInstanceBase::Setup(origShader);

	// copy effect pointer
	this->effect = vkShader->GetVkEffect();

	const AnyFX::ProgramBase* program = this->effect->GetPrograms()[0];

	const eastl::vector<AnyFX::VariableBase*>& variables = this->effect->GetVariables();
	const eastl::vector<AnyFX::VarblockBase*>& varblocks = this->effect->GetVarblocks();
	for (unsigned i = 0; i < varblocks.size(); i++)
	{
		AnyFX::VarblockBase* block = varblocks[i];

		bool usedBySystem = false;
		if (block->HasAnnotation("System")) usedBySystem = block->GetAnnotationBool("System");

		// only create buffer if it's not using a reserved block
		if (!usedBySystem && (program->activeVarblockNames.find(block->name) != program->activeVarblockNames.end()))
		{
			if (program->variableBlockOffsets.empty()) continue;

			// create uniform buffer
			Ptr<CoreGraphics::ConstantBuffer> uniformBuffer = CoreGraphics::ConstantBuffer::Create();
			
			// generate a name which we know will be unique
			Util::String name = block->name.c_str();
			n_assert(!this->uniformBuffersByName.Contains(name));

			// get variable corresponding to this block
			const Ptr<CoreGraphics::ShaderVariable>& blockVar = vkShader->GetVariableByName(name);

			// setup block with a single buffer
			uniformBuffer->SetSize(block->byteSize);
			uniformBuffer->Setup(1);

			uniformBuffer->BeginUpdateSync();
			for (unsigned j = 0; j < block->variables.size(); j++)
			{
				// find the shader variable and bind the constant buffer we just created to said variable
				const AnyFX::VariableBase* var = block->variables[j];
				eastl::map<eastl::string, unsigned>::const_iterator it = program->variableBlockOffsets.find(var->name.c_str());
				if (it != program->variableBlockOffsets.end())
				{
					//uint32_t offset = program->variableBlockOffsets[var->name.c_str()];
					uint32_t offset = it->second;
					Util::String name = var->name.c_str();
					uniformBuffer->UpdateArray(var->currentValue, offset, var->byteSize, 1);
					this->uniformVariableBinds.Add(name, VariableBufferBinding(DeferredVariableToBufferBind{ offset, var->byteSize, var->arraySize }, uniformBuffer));
				}
				
			}
			uniformBuffer->EndUpdateSync();

			// add to dictionaries
			this->uniformBuffers.Append(uniformBuffer);
			this->uniformBuffersByName.Add(name, uniformBuffer);
			this->blockToBufferBindings.Append(BlockBufferBinding(blockVar, uniformBuffer));
		}
	}
}

} // namespace Vulkan