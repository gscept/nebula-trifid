//------------------------------------------------------------------------------
//  skinnedmeshrendererbase.cc
//  (C) 2008 Radon Labs GmbH
//  (C) 2013-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "characters/base/skinnedmeshrendererbase.h"

namespace Base
{
__ImplementClass(Base::SkinnedMeshRendererBase, 'SMRB', Core::RefCounted);
__ImplementSingleton(Base::SkinnedMeshRendererBase);

using namespace Util;
using namespace Characters;
using namespace CoreGraphics;

//------------------------------------------------------------------------------
/**
*/
SkinnedMeshRendererBase::SkinnedMeshRendererBase() :
    inFrame(false),
    isValid(false),
    inGather(false),
    inUpdateJointTexture(false)
{
    __ConstructSingleton;
}

//------------------------------------------------------------------------------
/**
*/
SkinnedMeshRendererBase::~SkinnedMeshRendererBase()
{
    if (this->IsValid())
    {
        this->Discard();
    }
    __DestructSingleton;
}

//------------------------------------------------------------------------------
/**
*/
void
SkinnedMeshRendererBase::Setup()
{
    n_assert(!this->IsValid());
    this->isValid = true;
    this->inFrame = false;
    this->inGather = false;
    this->drawInfos.Clear();
}

//------------------------------------------------------------------------------
/**
*/
void
SkinnedMeshRendererBase::Discard()
{
    n_assert(this->IsValid());
    this->isValid = false;
    this->drawInfos.Clear();
}

//------------------------------------------------------------------------------
/**
*/
void
SkinnedMeshRendererBase::OnBeginFrame()
{
    n_assert(!this->inFrame);
    n_assert(!this->inGather);
    this->inFrame = true;
}

//------------------------------------------------------------------------------
/**
*/
void
SkinnedMeshRendererBase::OnEndFrame()
{
    n_assert(this->inFrame);
    n_assert(!this->inGather);
    this->inFrame = false;
    this->drawInfos.Clear();
    this->keyIndexMap.Clear();
}   

//------------------------------------------------------------------------------
/**
*/
void
SkinnedMeshRendererBase::BeginGatherSkins()
{
    n_assert(this->inFrame);
    n_assert(!this->inGather);
    n_assert(this->drawInfos.IsEmpty());
    this->inGather = true;
}

//------------------------------------------------------------------------------
/**
    This method should only be called when RequiresSoftwareSkinning()
    returns true!

    This registers a mesh for software-skinning in the 
    UpdateSoftwareSkinnedMeshes() which must be called after EndGatherSkins().

    This method may be called more then once per character-instance/mesh
    combination! The method will drop duplicates.
*/
SkinnedMeshRendererBase::DrawHandle
SkinnedMeshRendererBase::RegisterSoftwareSkinnedMesh(const Ptr<Characters::CharacterInstance>& charInst, const Ptr<CoreGraphics::Mesh>& mesh)
{
    n_assert(this->inFrame);
    n_assert(this->inGather);
    
    // check if charInst/mesh was already updated this frame
    IndexT drawInfoIndex;
    DrawKey key(charInst.get(), mesh.get());
    IndexT mapIndex = this->keyIndexMap.FindIndex(key);
    if (InvalidIndex == mapIndex)
    {
        // this is a new skin
        SkinnedMeshDrawInfo drawInfo;
        drawInfo.charInst = charInst;
        drawInfo.mesh = mesh;
        this->drawInfos.Append(drawInfo);
        drawInfoIndex = this->drawInfos.Size() - 1;                
        this->keyIndexMap.Add(key, drawInfoIndex);
    }
    else
    {
        drawInfoIndex = this->keyIndexMap.ValueAtIndex(mapIndex);
    }
    return drawInfoIndex;
}

//------------------------------------------------------------------------------
/**
*/
void
SkinnedMeshRendererBase::EndGatherSkins()
{
    n_assert(this->inFrame);
    n_assert(this->inGather);
    this->inGather = false;
}

//------------------------------------------------------------------------------
/**
    On platforms with software-skinning, this method should perform the
    skinning for all meshes gathered during the GatherSkins pass.
*/
void
SkinnedMeshRendererBase::UpdateSoftwareSkinnedMeshes()
{
    n_error("SkinnedMeshRendererBase::UpdateSoftwareSkinnedMesh() called!\n");
}
        
//------------------------------------------------------------------------------
/**
	Applies shader variables before rendering skinned mesh
*/
void 
SkinnedMeshRendererBase::UpdateGPUSkinnedJointPalette(const Ptr<Characters::CharacterInstance>& charInst, const Util::Array<IndexT>& jointPalette, const Ptr<CoreGraphics::ShaderVariable>& jointPaletteShdVar)
{
	n_error("SkinnedMeshRendererBase::UpdateGPUSkinnedMesh() called!\n");
}

//------------------------------------------------------------------------------
/**
	Updates joint texture for character
*/
void 
SkinnedMeshRendererBase::UpdateGPUTextureSkinnedJointPalette(const Ptr<Characters::CharacterInstance>& charInst, const Ptr<CoreGraphics::ShaderVariable>& jointPaletteShdVar)
{
	n_error("SkinnedMeshRendererBase::UpdateGPUTextureSkinnedMesh() called!\n");
}

//------------------------------------------------------------------------------
/**
    This method should only be called when RequiresSoftwareSkinning()
    returns true!

    Software-skinning platforms call this method with the DrawHandle returned
    by UpdateSoftwareSkinnedMesh() to draw a portion of the skinned mesh/
*/
void 
SkinnedMeshRendererBase::DrawSoftwareSkinnedMesh(DrawHandle h, IndexT primGroupIndex)
{
    n_error("SkinnedMeshRendererBase::DrawSkinnedMesh() called!");
}

//------------------------------------------------------------------------------
/**
    This method should only be called when RequiresSoftwareSkinning()
    returns false!

    This is the skinned-mesh rendering method for platforms which do skinning
    on the GPU.
*/
void
SkinnedMeshRendererBase::DrawGPUSkinnedMesh(const Ptr<CoreGraphics::Mesh>& mesh, IndexT primGroupIndex)
{
    n_error("SkinnedMeshRendererBase::DrawGPUSkinnedMesh() called!");
}

//------------------------------------------------------------------------------
/**
*/
void 
SkinnedMeshRendererBase::DrawGPUSkinnedMeshInstanced(const Ptr<CoreGraphics::Mesh>& mesh, IndexT primGroupIndex, SizeT numInstances)
{
    n_error("SkinnedMeshRendererBase::DrawGPUSkinnedMeshInstanced() called!");
}

//------------------------------------------------------------------------------
/**
*/
void*
SkinnedMeshRendererBase::AcquireJointTextureRowPointer(const Ptr<CharacterInstance>& charInst, SizeT& outRowPitch)
{
    return 0;
}

//------------------------------------------------------------------------------
/**
*/
IndexT 
SkinnedMeshRendererBase::AllocJointTextureRow()
{
    n_error("SkinnedMeshRendererBase::AllocJointTextureRow() called!");
    return InvalidIndex;
}

//------------------------------------------------------------------------------
/**
*/
void 
SkinnedMeshRendererBase::FreeJointTextureRow(IndexT characterIndex)
{
    n_error("SkinnedMeshRendererBase::FreeJointTextureRow() called!");
}

//------------------------------------------------------------------------------
/**
*/
void 
SkinnedMeshRendererBase::DrawGPUTextureSkinnedMesh(const Ptr<CoreGraphics::Mesh>& mesh, IndexT primGroupIndex)
{
    n_error("SkinnedMeshRendererBase::DrawGPUTextureSkinnedMesh() called!");
}

//------------------------------------------------------------------------------
/**
*/
void 
SkinnedMeshRendererBase::DrawGPUTextureSkinnedMeshInstanced(const Ptr<CoreGraphics::Mesh>& mesh, IndexT primGroupIndex, SizeT numInstanced)
{
    n_error("SkinnedMeshRendererBase::DrawGPUTextureSkinnedMeshInstanced() called!");
}

} // namespace Base