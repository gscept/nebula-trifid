//------------------------------------------------------------------------------
//  charactermaterialskinnodeinstance.cc
//  (C) 2011-2015 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "characters/characterskinnodeinstance.h"
#include "characters/characterskinnode.h"
#include "resources/managedmesh.h"
#include "models/modelinstance.h"
#include "characters/characternodeinstance.h"
#include "characters/characterserver.h"
#include "coregraphics/shader.h"
#include "coregraphics/renderdevice.h"
#include "coregraphics/shadersemantics.h"
#include "coregraphics/shaderserver.h"
#include "materials/materialinstance.h"
#include "materials/material.h"

namespace Characters
{
__ImplementClass(Characters::CharacterSkinNodeInstance, 'CSNI', Models::ShapeNodeInstance);

using namespace Util;
using namespace Resources;
using namespace CoreGraphics;
using namespace Materials;
using namespace Models;

//------------------------------------------------------------------------------
/**
*/
CharacterSkinNodeInstance::CharacterSkinNodeInstance()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
CharacterSkinNodeInstance::~CharacterSkinNodeInstance()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
CharacterSkinNodeInstance::Setup(const Ptr<ModelInstance>& inst, 
                                 const Ptr<ModelNode>& node, 
                                 const Ptr<ModelNodeInstance>& parentNodeInst)
{
    // need to call parent class first
    ShapeNodeInstance::Setup(inst, node, parentNodeInst);

    // lookup pointer to our character instance object
    this->characterInstance = inst->GetRootNodeInstance().downcast<CharacterNodeInstance>()->GetCharacterInstance();

	Ptr<CharacterServer> charServer = CharacterServer::Instance();
	SkinningTechnique::Code skinTech = charServer->GetSkinningTechnique();
	if (skinTech == SkinningTechnique::GPUSkinningFeedback)
	{
		// lookup skinning shader to use for transform feedbacks, and set relevant transform feedback
		this->skinningShader = ShaderServer::Instance()->GetShader("shd:skinned");
		this->skinningShader->SelectActiveVariation(ShaderServer::Instance()->FeatureStringToMask("Skinned|Feedback"));
		this->skinningJointPaletteVar = this->skinningShader->GetVariableByName(NEBULA3_SEMANTIC_JOINTPALETTE);

		// get mesh and primitive group
		const Ptr<CharacterSkinNode>& charNode = this->modelNode.cast<CharacterSkinNode>();
		const Ptr<ManagedMesh> managedMesh = charNode->GetManagedMesh();
		n_assert(managedMesh->GetState() == Resource::Loaded);
		const Ptr<Mesh>& mesh = managedMesh->GetMesh();
		const PrimitiveGroup& primGroup = mesh->GetPrimitiveGroupAtIndex(charNode->GetFragmentPrimGroupIndex(0));

		Array<VertexComponent> components;
		components.Append(VertexComponent(VertexComponent::Position, 0, VertexComponent::Float3, 0));
		components.Append(VertexComponent(VertexComponent::Normal, 0, VertexComponent::Float3, 0));
		components.Append(VertexComponent(VertexComponent::TexCoord1, 0, VertexComponent::Float2, 0));
		components.Append(VertexComponent(VertexComponent::Tangent, 0, VertexComponent::Float3, 0));
		components.Append(VertexComponent(VertexComponent::Binormal, 0, VertexComponent::Float3, 0));

		// create buffer to save feedback to
		this->feedbackBuffer = FeedbackBuffer::Create();
		this->feedbackBuffer->SetNumPrimitives(primGroup.GetNumPrimitives());
		this->feedbackBuffer->SetVertexComponents(components);
		this->feedbackBuffer->SetPrimitiveType(PrimitiveTopology::TriangleList);
		this->feedbackBuffer->Setup();
	}
	else if (skinTech == SkinningTechnique::GPUSkinning)
	{
		this->skinningShader = ShaderServer::Instance()->GetShader("shd:skinned");
		this->skinningJointPaletteVar = this->skinningShader->GetVariableByName(NEBULA3_SEMANTIC_JOINTBLOCK);

		// setup joint buffer
		const Ptr<CharacterSkinNode>& charNode = this->modelNode.cast<CharacterSkinNode>();

		// create the constant buffer representing our joints
		// it doesn't need to be synced since nobody will notice if the same animation frame repeats
		this->jointBuffer = ShaderReadWriteBuffer::Create();
		this->jointBuffer->SetSize(charNode->GetFragmentJointPalette(0).Size() * sizeof(Math::matrix44));
		this->jointBuffer->Setup();
	}
}

//------------------------------------------------------------------------------
/**
*/
void
CharacterSkinNodeInstance::Discard()
{
    this->characterInstance = 0;

	Ptr<CharacterServer> charServer = CharacterServer::Instance();
	if (charServer->GetSkinningTechnique() == SkinningTechnique::GPUSkinningFeedback)
	{
		this->feedbackBuffer->Discard();
		this->feedbackBuffer = 0;
		this->skinningJointPaletteVar = 0;
        this->skinningShader = 0;		
	}
	else if (charServer->GetSkinningTechnique() == SkinningTechnique::GPUSkinning)
	{
		this->jointBuffer->Discard();
		this->jointBuffer = 0;
		this->skinningJointPaletteVar = 0;
		this->skinningShader = 0;
	}
    ShapeNodeInstance::Discard();
}

//------------------------------------------------------------------------------
/**
*/
void
CharacterSkinNodeInstance::OnNotifyCullingVisible(IndexT frameIndex, Timing::Time time)
{
    // on software-skinned platforms, we need to update the skin mesh
    Ptr<CharacterServer> charServer = CharacterServer::Instance();
    if (SkinningTechnique::SoftwareSkinning == charServer->GetSkinningTechnique())
    {
        this->drawHandle = SkinnedMeshRenderer::InvalidDrawHandle;
        const Ptr<CharacterSkinNode>& myNode = this->modelNode.cast<CharacterSkinNode>();
        const Ptr<ManagedMesh> managedMesh = myNode->GetManagedMesh();
        if (managedMesh->GetState() == Resource::Loaded)
        {
            const Ptr<Mesh>& mesh = managedMesh->GetMesh();
            this->drawHandle = charServer->GatherSkinMesh(this->characterInstance, mesh);
        }
    }
    ShapeNodeInstance::OnNotifyCullingVisible(frameIndex, time);       
}

//------------------------------------------------------------------------------
/**
*/
void
CharacterSkinNodeInstance::OnRenderBefore(IndexT frameIndex, Timing::Time time)
{
	Ptr<RenderDevice> renderDev = RenderDevice::Instance();
	Ptr<CharacterServer> charServer = CharacterServer::Instance();
	SkinningTechnique::Code skinTech = charServer->GetSkinningTechnique();

	if (SkinningTechnique::GPUSkinningFeedback == skinTech)
	{
		// get mesh and primitive group
		const Ptr<CharacterSkinNode>& charNode = this->modelNode.cast<CharacterSkinNode>();
		const Ptr<ManagedMesh> managedMesh = charNode->GetManagedMesh();
		const Ptr<Mesh>& mesh = managedMesh->GetMesh();

		// get fragment joint palette			
		const Array<IndexT>& jointPalette = charNode->GetFragmentJointPalette(0);

		// get primitive index
		IndexT primIndex = charNode->GetFragmentPrimGroupIndex(0);

		// update joint palette, then draw into the feedback buffer
		//renderDev->SetPrimitiveGroup(this->feedbackBuffer->GetPrimitiveGroup());
		this->feedbackBuffer->Swap();
		charServer->UpdateGPUSkinnedJointPalette(this->characterInstance, jointPalette, this->skinningJointPaletteVar);
		renderDev->BeginFeedback(this->feedbackBuffer, mesh->GetPrimitiveGroupAtIndex(primIndex).GetPrimitiveTopology(), this->skinningShader);
		mesh->ApplyPrimitives(primIndex);
		renderDev->Draw();
		renderDev->EndFeedback();
	}
	else if (SkinningTechnique::GPUSkinning == skinTech)
	{
		// get mesh and primitive group
		const Ptr<CharacterSkinNode>& charNode = this->modelNode.cast<CharacterSkinNode>();
		const FixedArray<Math::matrix44>& skinMatrixArray = this->characterInstance->Skeleton().GetSkinMatrixArray();
		const Array<IndexT>& indices = charNode->GetFragmentJointPalette(0);
		FixedArray<Math::matrix44> joints(indices.Size());

		IndexT i;
		SizeT numJointsInPalette = indices.Size();
		for (i = 0; i < numJointsInPalette; i++)
		{
			joints[i] = skinMatrixArray[indices[i]];
		}

		// update joint buffer
		this->jointBuffer->Update(&joints[0], 0, joints.Size() * sizeof(Math::matrix44));
	}

	ShapeNodeInstance::OnRenderBefore(frameIndex, time);
}

//------------------------------------------------------------------------------
/**
*/
void 
CharacterSkinNodeInstance::ApplyState(IndexT frameIndex, const Frame::BatchGroup::Code& group, const Ptr<CoreGraphics::Shader>& shader)
{
	// different code paths for software and GPU-skinned platforms
	ShaderServer* shaderServer = ShaderServer::Instance();
	CharacterServer* charServer = CharacterServer::Instance();
	SkinningTechnique::Code skinTech = charServer->GetSkinningTechnique();

	if (SkinningTechnique::GPUTextureSkinning == skinTech)
	{           
        if (shader->HasVariableByName(NEBULA3_SEMANTIC_CHARACTERINDEX))
		{
			// get shader variable
            const Ptr<ShaderVariable>& var = shader->GetVariableByName(NEBULA3_SEMANTIC_CHARACTERINDEX);

			// update texture
			charServer->UpdateGPUSkinnedTextureJointPalette(this->characterInstance, var);
		}
	}
	else if (SkinningTechnique::GPUSkinning == skinTech)
	{
		// set handle
		this->skinningJointPaletteVar->SetBufferHandle(this->jointBuffer->GetHandle());
	}

	// apply base level state
	ShapeNodeInstance::ApplyState(frameIndex, group, shader);
}

//------------------------------------------------------------------------------
/**
*/
void
CharacterSkinNodeInstance::Render()
{   
    // different code paths for software and GPU-skinned platforms
	Ptr<ShaderServer> shaderServer = ShaderServer::Instance();
	Ptr<CharacterServer> charServer = CharacterServer::Instance();
	Ptr<RenderDevice> renderDev = RenderDevice::Instance();

    SkinningTechnique::Code skinTech = charServer->GetSkinningTechnique();
    if (SkinningTechnique::SoftwareSkinning == skinTech)
    {
        // software-skinned path (e.g. on Wii)
        if (this->drawHandle != SkinnedMeshRenderer::InvalidDrawHandle)
        {        
            const Ptr<CharacterSkinNode>& myNode = this->modelNode.cast<CharacterSkinNode>();
            IndexT fragIndex;
            SizeT numFragments = myNode->GetNumFragments();
            for (fragIndex = 0; fragIndex < numFragments; fragIndex++)
            {
                IndexT primGroupIndex = myNode->GetFragmentPrimGroupIndex(fragIndex);
                charServer->DrawSoftwareSkinnedMesh(this->drawHandle, primGroupIndex);
            }
        }
    }
    // use of texture as joint palette buffer
    else if (SkinningTechnique::GPUTextureSkinning == skinTech)
    {           
        // GPU-skinned path
        const Ptr<CharacterSkinNode>& myNode = this->modelNode.cast<CharacterSkinNode>();
        const Ptr<ManagedMesh> managedMesh = myNode->GetManagedMesh();

        if (managedMesh->GetState() == Resource::Loaded)
        {
			// get mesh
            const Ptr<Mesh>& mesh = managedMesh->GetMesh();

            // mesh available, render actual skin fragments
            SizeT numFragments = myNode->GetNumFragments();
            n_assert(numFragments > 0);
            IndexT primGroupIndex = myNode->GetFragmentPrimGroupIndex(0);    

			// draw skin
            charServer->DrawGPUTextureSkinnedMesh(mesh, primGroupIndex);            
        }
    }
	else if (SkinningTechnique::GPUSkinning == skinTech)
    {
        // GPU-skinned path
        const Ptr<CharacterSkinNode>& myNode = this->modelNode.cast<CharacterSkinNode>();
        const Ptr<ManagedMesh> managedMesh = myNode->GetManagedMesh();

		if (managedMesh->GetState() == Resource::Loaded)
		{
			// get mesh
			const Ptr<Mesh>& mesh = managedMesh->GetMesh();

			// mesh available, render actual skin fragments
			SizeT numFragments = myNode->GetNumFragments();
			n_assert(numFragments > 0);

			// get fragment
			IndexT primGroupIndex = myNode->GetFragmentPrimGroupIndex(0);

			// draw skin
			charServer->DrawGPUSkinnedMesh(mesh, primGroupIndex);
		}
	}
	else if (SkinningTechnique::GPUSkinningFeedback == skinTech)
	{
		// GPU-skinned path using transform feedbacks
		const Ptr<CharacterSkinNode>& myNode = this->modelNode.cast<CharacterSkinNode>();
		const Ptr<ManagedMesh> managedMesh = myNode->GetManagedMesh();

		if (managedMesh->GetState() == Resource::Loaded)
		{
			// get mesh
			const Ptr<Mesh>& mesh = managedMesh->GetMesh();

			// mesh available, render actual skin fragments
			SizeT numFragments = myNode->GetNumFragments();
			n_assert(numFragments > 0);

			// get fragment
			IndexT primGroupIndex = myNode->GetFragmentPrimGroupIndex(0);

			// render
			renderDev->SetPrimitiveGroup(this->feedbackBuffer->GetPrimitiveGroup());
			renderDev->SetVertexLayout(this->feedbackBuffer->GetLayout());
			renderDev->DrawFeedback(this->feedbackBuffer);
		}
	}
}

//------------------------------------------------------------------------------
/**
*/
void
CharacterSkinNodeInstance::RenderInstanced(SizeT numInstances)
{
	// different code paths for software and GPU-skinned platforms
	Ptr<ShaderServer> shaderServer = ShaderServer::Instance();
	Ptr<CharacterServer> charServer = CharacterServer::Instance();
	Ptr<RenderDevice> renderDev = RenderDevice::Instance();
	SkinningTechnique::Code skinTech = charServer->GetSkinningTechnique();
	n_assert(SkinningTechnique::GPUSkinning == skinTech || SkinningTechnique::GPUTextureSkinning);

	// GPU-skinned path
	const Ptr<CharacterSkinNode>& myNode = this->modelNode.cast<CharacterSkinNode>();
	const Ptr<ManagedMesh> managedMesh = myNode->GetManagedMesh();

	if (SkinningTechnique::GPUTextureSkinning == skinTech)
	{
		if (managedMesh->GetState() == Resource::Loaded)
		{
			// get mesh
			const Ptr<Mesh>& mesh = managedMesh->GetMesh();

			// mesh available, render actual skin fragments
			SizeT numFragments = myNode->GetNumFragments();
			n_assert(numFragments > 0);
			IndexT primGroupIndex = myNode->GetFragmentPrimGroupIndex(0);

			// draw skin
			charServer->DrawGPUTextureSkinnedMeshInstanced(mesh, primGroupIndex, numInstances);
		}
	}
	else if (SkinningTechnique::GPUSkinning == skinTech)
	{
		if (managedMesh->GetState() == Resource::Loaded)
		{
			// get mesh
			const Ptr<Mesh>& mesh = managedMesh->GetMesh();

			// mesh available, render actual skin fragments
			SizeT numFragments = myNode->GetNumFragments();
			n_assert(numFragments > 0);

			// get fragment
			IndexT primGroupIndex = myNode->GetFragmentPrimGroupIndex(0);

			// draw skin
			charServer->DrawGPUSkinnedMeshInstanced(mesh, primGroupIndex, numInstances);
		}
	}
	else if (SkinningTechnique::GPUSkinningFeedback == skinTech)
	{
		if (managedMesh->GetState() == Resource::Loaded)
		{
			// get mesh
			const Ptr<Mesh>& mesh = managedMesh->GetMesh();

			// mesh available, render actual skin fragments
			SizeT numFragments = myNode->GetNumFragments();
			n_assert(numFragments > 0);

			// get fragment
			IndexT primGroupIndex = myNode->GetFragmentPrimGroupIndex(0);

			// render
			renderDev->SetPrimitiveGroup(this->feedbackBuffer->GetPrimitiveGroup());
			renderDev->SetVertexLayout(this->feedbackBuffer->GetLayout());
			renderDev->DrawFeedbackInstanced(this->feedbackBuffer, numInstances);
		}
	}
}
} // namespace Characters
