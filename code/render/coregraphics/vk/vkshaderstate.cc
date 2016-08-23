//------------------------------------------------------------------------------
// vkshaderstate.cc
// (C) 2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "vkshaderstate.h"
#include "coregraphics/constantbuffer.h"
#include "coregraphics/shader.h"
#include "vkrenderdevice.h"
#include "coregraphics/shadervariable.h"

using namespace CoreGraphics;
namespace Vulkan
{

__ImplementClass(Vulkan::VkShaderState, 'VKSN', Base::ShaderStateBase);
//------------------------------------------------------------------------------
/**
*/
VkShaderState::VkShaderState() :
	pushData(NULL),
	pushSize(0)
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
VkShaderState::~VkShaderState()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
void
VkShaderState::Setup(const Ptr<CoreGraphics::Shader>& origShader)
{
	n_assert(origShader.isvalid());
	const Ptr<VkShader>& vkShader = origShader.upcast<VkShader>();

	// call parent class
	ShaderStateBase::Setup(origShader);

	// copy effect pointer
	this->effect = vkShader->GetVkEffect();

	// setup variables using empty groups list
	if (!origShader->setLayouts.IsEmpty())
	{
		Util::Array<IndexT> groups = this->shader->setBindings.KeysAsArray();
		this->SetupVariables(groups);
		this->SetupUniformBuffers(groups);
	}
}

//------------------------------------------------------------------------------
/**
*/
void
VkShaderState::Setup(const Ptr<CoreGraphics::Shader>& origShader, const Util::Array<IndexT>& groups)
{
	n_assert(origShader.isvalid());
	const Ptr<VkShader>& vkShader = origShader.upcast<VkShader>();

	// call parent class
	ShaderStateBase::Setup(origShader);

	// copy effect pointer
	this->effect = vkShader->GetVkEffect();

	// setup variables if we have any layouts
	if (!origShader->setLayouts.IsEmpty())
	{
		this->SetupVariables(groups);
		this->SetupUniformBuffers(groups);
	}	
}

//------------------------------------------------------------------------------
/**
*/
void
VkShaderState::BeginUpdateSync()
{
	IndexT i;
	for (i = 0; i < this->shader->buffers.Size(); i++)
	{
		this->shader->buffers.ValueAtIndex(i)->BeginUpdateSync();
	}
}

//------------------------------------------------------------------------------
/**
*/
void
VkShaderState::EndUpdateSync()
{
	IndexT i;
	for (i = 0; i < this->shader->buffers.Size(); i++)
	{
		this->shader->buffers.ValueAtIndex(i)->EndUpdateSync();
	}
}


//------------------------------------------------------------------------------
/**
*/
void
VkShaderState::Apply()
{
	this->shader->Apply();
	ShaderStateBase::Apply();
}

//------------------------------------------------------------------------------
/**
*/
void
VkShaderState::Commit()
{
	this->UpdateDescriptorSets();

	// get render device to apply state
	VkRenderDevice* dev = VkRenderDevice::Instance();

	// now go through and make sure the shader can bind the sets updated
	if (this->shader->activeVariation.isvalid())
	{
		const VkShaderProgram::PipelineType type = this->shader->activeVariation->GetPipelineType();
		n_assert(type != VkShaderProgram::InvalidType);
		if (type == VkShaderProgram::Graphics)
		{
			// if no variation is being used, bind descriptors for both graphics and compute
			IndexT i;
			for (i = 0; i < this->setBindnings.Size(); i++)
			{
				const DescriptorSetBinding& binding = this->setBindnings[i];
				const Util::Array<uint32_t>& offsets = this->offsetsByGroup[binding.slot];
				dev->BindDescriptorsGraphics(&binding.set, binding.layout, binding.slot, 1, offsets.IsEmpty() ? NULL : &offsets[0], offsets.Size());
			}

			// update push ranges
			if (this->pushSize > 0)
			{
				dev->UpdatePushRanges(VK_SHADER_STAGE_ALL, this->pushLayout, 0, this->pushSize, this->pushData);
			}
		}
		else
		{
			// if no variation is being used, bind descriptors for both graphics and compute
			IndexT i;
			for (i = 0; i < this->setBindnings.Size(); i++)
			{
				const DescriptorSetBinding& binding = this->setBindnings[i];
				const Util::Array<uint32_t>& offsets = this->offsetsByGroup[binding.slot];
				dev->BindDescriptorsCompute(&binding.set, binding.layout, binding.slot, 1, offsets.IsEmpty() ? NULL : &offsets[0], offsets.Size());
			}

			// update push ranges
			if (this->pushSize > 0)
			{
				dev->UpdatePushRanges(VK_SHADER_STAGE_ALL, this->pushLayout, 0, this->pushSize, this->pushData);
			}
		}
	}
	else
	{
		// if no variation is being used, bind descriptors for both graphics and compute
		IndexT i;
		for (i = 0; i < this->setBindnings.Size(); i++)
		{
			const DescriptorSetBinding& binding = this->setBindnings[i];
			const Util::Array<uint32_t>& offsets = this->offsetsByGroup[binding.slot];
			dev->BindDescriptorsGraphics(&binding.set, binding.layout, binding.slot, 1, offsets.IsEmpty() ? NULL : &offsets[0], offsets.Size());
			dev->BindDescriptorsCompute(&binding.set, binding.layout, binding.slot, 1, offsets.IsEmpty() ? NULL : &offsets[0], offsets.Size());
		}

		// push to both compute and graphics
		if (this->pushSize > 0)
		{
			dev->UpdatePushRanges(VK_SHADER_STAGE_ALL, this->pushLayout, 0, this->pushSize, this->pushData);
		}
	}
}

//------------------------------------------------------------------------------
/**
*/
void
VkShaderState::AddDescriptorWrite(const VkWriteDescriptorSet& write)
{
	this->pendingSetWrites.Append(write);
}

static int NumSetsAllocated = 0;
static int NumDescriptorsAllocated = 0;
//------------------------------------------------------------------------------
/**
*/
void
VkShaderState::SetupVariables(const Util::Array<IndexT>& groups)
{
	// get layouts associated with group and setup descriptor sets for them
	Util::FixedArray<VkDescriptorSetLayout> setLayouts;

	// copy sets from shader
	this->sets.Resize(groups.Size());
	this->setBindnings.Resize(groups.Size());
	IndexT i;
	for (i = 0; i < groups.Size(); i++)
	{
		this->sets[i] = this->shader->sets[groups[i]];
	}

	// setup binds, we will use there later when applying the shader
	for (i = 0; i < groups.Size(); i++)
	{
		DescriptorSetBinding binding;
#if AMD_DESC_SETS
		binding.set = this->sets[i];
#else
		binding.set = this->sets[this->shader->setToIndexMap[i]];
#endif
		binding.slot = groups[i];
		//binding.layout = this->shader->pipelineSetLayouts[groups[i]];
		binding.layout = this->shader->pipelineLayout;
		this->setBindnings[i] = binding;
	}

	/// setup variables for the groups this shader should modify
	for (i = 0; i < groups.Size(); i++)
	{
		const AnyFX::ProgramBase* program = this->effect->GetPrograms()[0];

		// get handles related to groups
		const eastl::vector<AnyFX::VariableBase*>& variables = this->effect->GetVariables(groups[i]);
		const eastl::vector<AnyFX::VarblockBase*>& varblocks = this->effect->GetVarblocks(groups[i]);
		const eastl::vector<AnyFX::VarbufferBase*>& varbuffers = this->effect->GetVarbuffers(groups[i]);

		// load uniforms
		uint j;
		for (j = 0; j < variables.size(); j++)
		{
			// get AnyFX variable
			AnyFX::VkVariable* variable = static_cast<AnyFX::VkVariable*>(variables[j]);

			// create new variable
			Ptr<ShaderVariable> var = ShaderVariable::Create();

			// setup variable from AnyFX variable
			var->Setup(variable, this, this->sets[i]);
			this->variables.Append(var);
			this->variablesByName.Add(variable->name.c_str(), var);
		}

		// load shader storage buffer variables
		for (j = 0; j < varblocks.size(); j++)
		{
			// get AnyFX variable
			AnyFX::VkVarblock* block = static_cast<AnyFX::VkVarblock*>(varblocks[j]);
			if (block->variables.empty() || block->push) continue;

			// create new variable
			Ptr<ShaderVariable> var = ShaderVariable::Create();

			// setup variable from AnyFX varblock
			var->Setup(block, this, this->sets[i]);
			this->variables.Append(var);
			this->variablesByName.Add(block->name.c_str(), var);
		}

		// load uniform block variables
		for (j = 0; j < varbuffers.size(); j++)
		{
			// get varblock
			AnyFX::VkVarbuffer* buffer = static_cast<AnyFX::VkVarbuffer*>(varbuffers[j]);

			// create a new variable
			Ptr<ShaderVariable> var = ShaderVariable::Create();

			// setup variable from AnyFX varbuffer
			var->Setup(buffer, this, this->sets[i]);
			this->variables.Append(var);
			this->variablesByName.Add(buffer->name.c_str(), var);
		}
	}	
}

//------------------------------------------------------------------------------
/**
*/
void
VkShaderState::SetupUniformBuffers(const Util::Array<IndexT>& groups)
{
	IndexT i;
	for (i = 0; i < groups.Size(); i++)
	{
		// get varblocks by group
		const eastl::vector<AnyFX::VarblockBase*>& varblocks = this->effect->GetVarblocks(groups[i]);
		Util::Array<uint32_t> offsets;
		for (uint j = 0; j < varblocks.size(); j++)
		{
			AnyFX::VarblockBase* block = varblocks[j];

			bool usedBySystem = false;
			if (block->HasAnnotation("System")) usedBySystem = block->GetAnnotationBool("System");

			// only create a buffer if it's not system and only if it's being used in any of the shader programs
			if (!usedBySystem && block->alignedSize > 0)
			{
				// generate a name which we know will be unique
				Util::String name = block->name.c_str();

				// if we have an ordinary uniform buffer, allocate space for it
				if (!block->push)
				{
					n_assert(this->shader->buffers.Contains(name));
					Ptr<CoreGraphics::ConstantBuffer> uniformBuffer = this->shader->buffers[name];
					const Ptr<CoreGraphics::ShaderVariable>& bufferVar = this->GetVariableByName(name);

					// allocate single instance within uniform buffer and get offset
					uint32_t instanceOffset = uniformBuffer->AllocateInstance();
					offsets.Append(instanceOffset);

					// add to dictionary so we can dealloc later
					this->instances.Add(uniformBuffer, instanceOffset);

					// update buffer
					uniformBuffer->BeginUpdateSync();
					for (uint k = 0; k < block->variables.size(); k++)
					{
						// find the shader variable and bind the constant buffer we just created to said variable
						const AnyFX::VariableBase* var = block->variables[k];
						Util::String name = var->name.c_str();
						unsigned varOffset = block->offsetsByName[var->name];
						const Ptr<CoreGraphics::ShaderVariable>& member = this->GetVariableByName(name);
						member->BindToUniformBuffer(uniformBuffer, instanceOffset + varOffset, var->byteSize, (int8_t*)var->currentValue);
					}
					uniformBuffer->EndUpdateSync();

					// we apply the constant buffer again, in case we have to grow the buffer and reallocate it
					bufferVar->SetConstantBuffer(uniformBuffer);
				}
				else
				{
					// we only allow 1 push range
					n_assert(this->pushData == NULL);

					// allocate push range
					this->pushData = n_new_array(uint8_t, block->alignedSize);
					this->pushSize = block->alignedSize;
					this->pushLayout = this->shader->pipelineLayout;
					for (uint k = 0; k < block->variables.size(); k++)
					{
						// find the shader variable and bind the constant buffer we just created to said variable
						const AnyFX::VariableBase* var = block->variables[k];
						Util::String name = var->name.c_str();
						unsigned varOffset = block->offsetsByName[var->name];
						const Ptr<CoreGraphics::ShaderVariable>& member = this->GetVariableByName(name);
						member->BindToPushConstantRange(this->pushData, varOffset, var->byteSize, (int8_t*)var->currentValue);
					}
				}
			}
			else
			{
				offsets.Append(0);
			}
		}
		this->offsetsByGroup.Add(groups[i], offsets);
	}

	// perform descriptor set update, since our buffers might grow, we might have pending updates, and since the old buffer is destroyed, we want to flush all updates here.
	this->UpdateDescriptorSets();
}

//------------------------------------------------------------------------------
/**
*/
void
VkShaderState::UpdateDescriptorSets()
{
	// first ensure descriptor sets are up to date with whatever the variable values has been set to
	// this can be destructive, because it changes the base shader state
	if (this->pendingSetWrites.Size() > 0)
	{
		vkUpdateDescriptorSets(VkRenderDevice::dev, this->pendingSetWrites.Size(), &this->pendingSetWrites[0], 0, NULL);
		this->pendingSetWrites.Clear();
	}
}

//------------------------------------------------------------------------------
/**
*/
void
VkShaderState::Discard()
{
	if (this->pushData != NULL) n_delete_array(this->pushData);

	// free instances
	IndexT i;
	for (i = 0; i < this->instances.Size(); i++)
	{
		const Ptr<CoreGraphics::ConstantBuffer>& buf = this->instances.KeyAtIndex(i);
		buf->FreeInstance(this->instances.ValueAtIndex(i));
	}
}

} // namespace Vulkan