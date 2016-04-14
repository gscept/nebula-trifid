#pragma once
//------------------------------------------------------------------------------
/**
    @class Characters::CharacterServer
  
    Handles central aspects of the character system.
    
    (C) 2009 Radon Labs GmbH
    (C) 2013-2016 Individual contributors, see AUTHORS file
*/    
#include "core/refcounted.h"
#include "core/singleton.h"
#include "characters/characterinstance.h"
#include "characters/skinningtechnique.h"
#include "characters/skinnedmeshrenderer.h"
#include "timing/time.h"

//------------------------------------------------------------------------------
namespace Characters
{
class CharacterServer : public Core::RefCounted
{
    __DeclareClass(CharacterServer);
    __DeclareSingleton(CharacterServer);
public:
    /// constructor
    CharacterServer();
    /// destructor
    virtual ~CharacterServer();
    
    /// setup the character server
    void Setup();
    /// discard the character server
    void Discard();
    /// return true if character server has been setup
    bool IsValid() const;

    /// get the skinning technique used by this platform
    SkinningTechnique::Code GetSkinningTechnique() const;    

    /// begin frame
    void BeginFrame(IndexT frameIndex);

    /// begin gathering phase (there may be several per frame)
    void BeginGather();
    /// register a visible character (same character may be registered several times per frame!)
    void GatherVisibleCharacter(const Ptr<CharacterInstance>& charInst, Timing::Time time);
    /// update a character skin
    SkinnedMeshRenderer::DrawHandle GatherSkinMesh(const Ptr<CharacterInstance>& charInst, const Ptr<CoreGraphics::Mesh>& srcMesh);
    /// finish gathering phase
    void EndGather();    

    /// update visible characters (update animation, evaluate skeleton, potentially perform software skinning)
    void StartUpdateCharacterSkeletons();
    /// update character skins (if software skinning)
    void UpdateCharacterSkins();

    /// begin render phase
    void BeginDraw();
    /// draw a software skinned mesh
    void DrawSoftwareSkinnedMesh(SkinnedMeshRenderer::DrawHandle h, IndexT primGroupIndex);
	/// updates palette of GPU skinned mesh
    void UpdateGPUSkinnedJointPalette(const Ptr<Characters::CharacterInstance>& charInst, const Util::Array<IndexT>& jointPalette, const Ptr<CoreGraphics::ShaderVariable>& jointPaletteShdVar);
	/// updates GPU texture skinned mesh
    void UpdateGPUSkinnedTextureJointPalette(const Ptr<Characters::CharacterInstance>& charInst, const Ptr<CoreGraphics::ShaderVariable>& jointPaletteShdVar);
    /// draw a hardware skinned mesh
    void DrawGPUSkinnedMesh(const Ptr<CoreGraphics::Mesh>& mesh, IndexT primGroupIndex);
    /// draw a hardware skinned mesh with instancing
    void DrawGPUSkinnedMeshInstanced(const Ptr<CoreGraphics::Mesh>& mesh, IndexT primGroupIndex, SizeT numInstances);
    /// draw a texture skinned mesh
    void DrawGPUTextureSkinnedMesh(const Ptr<CoreGraphics::Mesh>& mesh, IndexT primGroupIndex);
    /// draw a texture skinned mesh with instancing
    void DrawGPUTextureSkinnedMeshInstanced(const Ptr<CoreGraphics::Mesh>& mesh, IndexT primGroupIndex, SizeT numInstances);
    /// end render phase
    void EndDraw();

    /// end current frame
    void EndFrame();    

private:
    Ptr<SkinnedMeshRenderer> skinnedMeshRenderer;
    Util::Array<Ptr<CharacterInstance> > visCharInstArray;
    IndexT curFrameIndex;
    bool isValid;
    bool inFrame;
    bool inGather;
    bool inDraw;
};

//------------------------------------------------------------------------------
/**
*/
inline bool
CharacterServer::IsValid() const
{
    return this->isValid;
}

//------------------------------------------------------------------------------
/**
*/
inline SkinningTechnique::Code
CharacterServer::GetSkinningTechnique() const
{
    return this->skinnedMeshRenderer->GetSkinningTechnique();
}

} // namespace Characters
//------------------------------------------------------------------------------
