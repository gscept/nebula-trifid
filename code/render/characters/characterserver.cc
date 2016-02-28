//------------------------------------------------------------------------------
//  characterserver.cc
//  (C) 2009 Radon Labs GmbH
//  (C) 2013-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "characters/characterserver.h"

namespace Characters
{
__ImplementClass(Characters::CharacterServer, 'CHRS', Core::RefCounted);
__ImplementSingleton(Characters::CharacterServer);

using namespace Util;
using namespace CoreGraphics;

//------------------------------------------------------------------------------
/**
*/
CharacterServer::CharacterServer() :
    curFrameIndex(InvalidIndex),
    isValid(false),
    inFrame(false),
    inGather(false),
    inDraw(false)
{
    __ConstructSingleton;
}

//------------------------------------------------------------------------------
/**
*/
CharacterServer::~CharacterServer()
{
    n_assert(!this->isValid);
    __DestructSingleton;
}

//------------------------------------------------------------------------------
/**
*/
void
CharacterServer::Setup()
{
    n_assert(!this->isValid);
    this->isValid = true;
    this->skinnedMeshRenderer = SkinnedMeshRenderer::Create();
    this->skinnedMeshRenderer->Setup();
}

//------------------------------------------------------------------------------
/**
*/
void
CharacterServer::Discard()
{
    n_assert(this->isValid);
    n_assert(this->visCharInstArray.IsEmpty());
    this->skinnedMeshRenderer->Discard();
    this->skinnedMeshRenderer = 0;
    this->isValid = false;    
}

//------------------------------------------------------------------------------
/**
*/
void
CharacterServer::BeginFrame(IndexT frameIndex)
{
    n_assert(this->isValid);
    n_assert(!this->inFrame);
	// why do we need to assert this?
    //n_assert(this->curFrameIndex != frameIndex);
    n_assert(this->visCharInstArray.IsEmpty());

    this->inFrame = true;
    this->curFrameIndex = frameIndex;
    this->skinnedMeshRenderer->OnBeginFrame();
}

//------------------------------------------------------------------------------
/**
*/
void
CharacterServer::EndFrame()
{
    n_assert(this->inFrame);
    this->inFrame = false;
    this->visCharInstArray.Clear();
    this->skinnedMeshRenderer->OnEndFrame();
}

//------------------------------------------------------------------------------
/**
*/
void
CharacterServer::BeginGather()
{
    n_assert(this->inFrame);
    n_assert(!this->inGather);
    n_assert(this->visCharInstArray.IsEmpty());

    this->inGather = true;
    this->skinnedMeshRenderer->BeginGatherSkins();
}

//------------------------------------------------------------------------------
/**
*/
void
CharacterServer::GatherVisibleCharacter(const Ptr<CharacterInstance>& charInst, Timing::Time time)
{
    n_assert(this->inGather);
        
    // only register this character as visible if it hasn't already been this frame
    // PrepareUpdate returns true if this is the first frame the function has been called
    if (charInst->PrepareUpdate(this->curFrameIndex))
    {
        this->visCharInstArray.Append(charInst);
    }
}

//------------------------------------------------------------------------------
/**
*/
SkinnedMeshRenderer::DrawHandle
CharacterServer::GatherSkinMesh(const Ptr<CharacterInstance>& charInst, const Ptr<CoreGraphics::Mesh>& srcMesh)
{
    n_assert(this->inGather);
    n_assert(this->skinnedMeshRenderer->GetSkinningTechnique() == SkinningTechnique::SoftwareSkinning);
    return this->skinnedMeshRenderer->RegisterSoftwareSkinnedMesh(charInst, srcMesh);
}

//------------------------------------------------------------------------------
/**
*/
void
CharacterServer::EndGather()
{
    n_assert(this->inGather);
    this->inGather = false;
    this->skinnedMeshRenderer->EndGatherSkins();
}

//------------------------------------------------------------------------------
/**
    Start updating character skeletons. This is an asynchronous operation!        
*/
void
CharacterServer::StartUpdateCharacterSkeletons()
{
    n_assert(!this->inGather)
    IndexT i;
    SizeT num = this->visCharInstArray.Size();
    for (i = 0; i < num; i++)
    {
        this->visCharInstArray[i]->StartUpdate();
    }
}

//------------------------------------------------------------------------------
/**
*/
void
CharacterServer::UpdateCharacterSkins()
{
    n_assert(!this->inGather);
    if (this->skinnedMeshRenderer->GetSkinningTechnique() == SkinningTechnique::SoftwareSkinning)
    {
        this->skinnedMeshRenderer->UpdateSoftwareSkinnedMeshes();
    }
}

//------------------------------------------------------------------------------
/**
*/
void
CharacterServer::BeginDraw()
{
    n_assert(this->isValid);
    n_assert(!this->inDraw);
    this->inDraw = true;
}

//------------------------------------------------------------------------------
/**
*/
void
CharacterServer::DrawSoftwareSkinnedMesh(SkinnedMeshRenderer::DrawHandle h, IndexT primGroupIndex)
{
    n_assert(this->inDraw);
    n_assert(this->skinnedMeshRenderer->GetSkinningTechnique() == SkinningTechnique::SoftwareSkinning);
    this->skinnedMeshRenderer->DrawSoftwareSkinnedMesh(h, primGroupIndex);
}

//------------------------------------------------------------------------------
/**
*/
void
CharacterServer::UpdateGPUSkinnedJointPalette(const Ptr<Characters::CharacterInstance>& charInst, const Util::Array<IndexT>& jointPalette, const Ptr<CoreGraphics::ShaderVariable>& jointPaletteShdVar)
{
	n_assert(this->inDraw);
	n_assert(this->skinnedMeshRenderer->GetSkinningTechnique() == SkinningTechnique::GPUSkinning || this->skinnedMeshRenderer->GetSkinningTechnique() == SkinningTechnique::GPUSkinningFeedback);
	this->skinnedMeshRenderer->UpdateGPUSkinnedJointPalette(charInst, jointPalette, jointPaletteShdVar);
}

//------------------------------------------------------------------------------
/**
*/
void 
CharacterServer::UpdateGPUSkinnedTextureJointPalette(const Ptr<Characters::CharacterInstance>& charInst, const Ptr<CoreGraphics::ShaderVariable>& jointPaletteShdVar)
{
	n_assert(this->inDraw);
	n_assert(this->skinnedMeshRenderer->GetSkinningTechnique() == SkinningTechnique::GPUTextureSkinning);
	this->skinnedMeshRenderer->UpdateGPUTextureSkinnedJointPalette(charInst, jointPaletteShdVar);
}

//------------------------------------------------------------------------------
/**
*/
void
CharacterServer::DrawGPUSkinnedMesh(const Ptr<Mesh>& mesh, IndexT primGroupIndex)
{
    n_assert(this->inDraw);
    n_assert(this->skinnedMeshRenderer->GetSkinningTechnique() == SkinningTechnique::GPUSkinning);
    this->skinnedMeshRenderer->DrawGPUSkinnedMesh(mesh, primGroupIndex);
}

//------------------------------------------------------------------------------
/**
*/
void 
CharacterServer::DrawGPUSkinnedMeshInstanced(const Ptr<CoreGraphics::Mesh>& mesh, IndexT primGroupIndex, SizeT numInstances)
{
    n_assert(this->inDraw);
    n_assert(this->skinnedMeshRenderer->GetSkinningTechnique() == SkinningTechnique::GPUSkinning);
    this->skinnedMeshRenderer->DrawGPUSkinnedMeshInstanced(mesh, primGroupIndex, numInstances);
}

//------------------------------------------------------------------------------
/**
*/
void
CharacterServer::DrawGPUTextureSkinnedMesh(const Ptr<Mesh>& mesh, IndexT primGroupIndex)
{
    n_assert(this->inDraw);
    n_assert(this->skinnedMeshRenderer->GetSkinningTechnique() == SkinningTechnique::GPUTextureSkinning);
    this->skinnedMeshRenderer->DrawGPUTextureSkinnedMesh(mesh, primGroupIndex);
}

//------------------------------------------------------------------------------
/**
*/
void 
CharacterServer::DrawGPUTextureSkinnedMeshInstanced(const Ptr<CoreGraphics::Mesh>& mesh, IndexT primGroupIndex, SizeT numInstances)
{
    n_assert(this->inDraw);
    n_assert(this->skinnedMeshRenderer->GetSkinningTechnique() == SkinningTechnique::GPUTextureSkinning);
    this->skinnedMeshRenderer->DrawGPUTextureSkinnedMeshInstanced(mesh, primGroupIndex, numInstances);
}

//------------------------------------------------------------------------------
/**
*/
void
CharacterServer::EndDraw()
{
    n_assert(this->inDraw);
    this->inDraw = false;
}

} // namespace Characters