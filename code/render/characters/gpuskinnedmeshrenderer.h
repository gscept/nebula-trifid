#pragma once
//------------------------------------------------------------------------------
/** 
    @class Characters::GPUSkinnedMeshRenderer
  
    GPU implemention of SkinnedMeshRenderer.
    
    (C) 2008 Radon Labs GmbH
    (C) 2013-2015 Individual contributors, see AUTHORS file
*/    
#include "characters/base/skinnedmeshrendererbase.h"
#include "util/stack.h"
#include "coregraphics/texture.h"


//------------------------------------------------------------------------------
namespace Characters
{
class GPUSkinnedMeshRenderer : public Base::SkinnedMeshRendererBase
{
    __DeclareClass(GPUSkinnedMeshRenderer);
    __DeclareSingleton(GPUSkinnedMeshRenderer);
public:
    /// constructor
    GPUSkinnedMeshRenderer();
    /// destructor
    virtual ~GPUSkinnedMeshRenderer();

    /// get the skinning technique used by the renderer
    Characters::SkinningTechnique::Code GetSkinningTechnique() const;

private:
    friend class Characters::CharacterServer;

	/// updates a skinned mesh
	void UpdateGPUSkinnedJointPalette(const Ptr<Characters::CharacterInstance>& charInst, const Util::Array<IndexT>& jointPalette, const Ptr<CoreGraphics::ShaderVariable>& jointPaletteShdVar);
    /// draw a skinned mesh
    void DrawGPUSkinnedMesh(const Ptr<CoreGraphics::Mesh>& mesh, IndexT primGroupIndex);
    /// draw skinned mesh instanced
    void DrawGPUSkinnedMeshInstanced(const Ptr<CoreGraphics::Mesh>& mesh, IndexT primGroupIndex, SizeT instances);

    static const SizeT ConstantBufferMaxJoints = 96;
};

//------------------------------------------------------------------------------
/**
*/
inline Characters::SkinningTechnique::Code
GPUSkinnedMeshRenderer::GetSkinningTechnique() const
{
    return Characters::SkinningTechnique::GPUSkinning;
}

} // namespace Characters
//------------------------------------------------------------------------------
