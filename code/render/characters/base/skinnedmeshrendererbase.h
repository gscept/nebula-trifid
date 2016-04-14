#pragma once
//------------------------------------------------------------------------------
/**
    @class Base::SkinnedMeshRendererBase
    
    Wraps platform-specific rendering of a skinned mesh. For a platform
    which supports GPU skinning (e.g. anything except the Wii), simply
    call the DrawGPUSkinnedMesh(). For a software-skinned platform,
    call the UpdateSoftwareSkinnedMesh() ideally once per frame (although the 
    method makes sure, that a mesh isn't skinned twice
    even when the method is called multiple times), and then use the returned
    DrawHandle to call DrawSoftwareSkinnedMesh() several times per frame.

    See CharacterNodeInstance for details.
    
    (C) 2008 Radon Labs GmbH
    (C) 2013-2016 Individual contributors, see AUTHORS file
*/
#include "core/refcounted.h"
#include "core/singleton.h"
#include "util/array.h"
#include "characters/skinnedmeshdrawinfo.h"
#include "characters/skinningtechnique.h"
#include "coregraphics/shadervariable.h"
#include "materials/materialvariable.h"
#include "coregraphics/texture.h"

namespace Characters
{
class CharacterServer;
class CharacterInstance;
}

//------------------------------------------------------------------------------
namespace Base
{
class SkinnedMeshRendererBase : public Core::RefCounted
{
    __DeclareClass(SkinnedMeshRendererBase);
    __DeclareSingleton(SkinnedMeshRendererBase);
public:
    /// an abstract draw handle
    typedef IndexT DrawHandle;
    static const DrawHandle InvalidDrawHandle = InvalidIndex;   

    /// constructor
    SkinnedMeshRendererBase();
    /// destructor
    virtual ~SkinnedMeshRendererBase();

    /// setup the renderer
    void Setup();
    /// discard the renderer
    void Discard();
    /// return true if renderer is valid
    bool IsValid() const;

    /// get the skinning technique used by the renderer
    Characters::SkinningTechnique::Code GetSkinningTechnique() const;

protected:
    friend class Characters::CharacterServer;
    friend class Characters::CharacterInstance;

    /// call once at beginning of frame
    void OnBeginFrame();
    /// call once at end of frame (after rendering)
    void OnEndFrame();

    /// begin gathering software-skinned meshes
    void BeginGatherSkins();
    /// update a software skinned mesh
    DrawHandle RegisterSoftwareSkinnedMesh(const Ptr<Characters::CharacterInstance>& charInst, const Ptr<CoreGraphics::Mesh>& mesh);
    /// end gathering software-skinned meshes
    void EndGatherSkins();
    /// update software-skinned meshes
    void UpdateSoftwareSkinnedMeshes();
	/// updates GPU skinned mesh joint palette
    void UpdateGPUSkinnedJointPalette(const Ptr<Characters::CharacterInstance>& charInst, const Util::Array<IndexT>& jointPalette, const Ptr<CoreGraphics::ShaderVariable>& jointPaletteShdVar);
	/// updates GPU texture skinned mesh joint palette
    void UpdateGPUTextureSkinnedJointPalette(const Ptr<Characters::CharacterInstance>& charInst, const Ptr<CoreGraphics::ShaderVariable>& jointPaletteShdVar);

    /// draw a software skinned mesh
    void DrawSoftwareSkinnedMesh(DrawHandle h, IndexT primGroupIndex);
    /// draw a hardware skinned mesh
    void DrawGPUSkinnedMesh(const Ptr<CoreGraphics::Mesh>& mesh, IndexT primGroupIndex);
    /// draw a hardware skinned mesh
    void DrawGPUSkinnedMeshInstanced(const Ptr<CoreGraphics::Mesh>& mesh, IndexT primGroupIndex, SizeT numInstances);
    /// draw a skinned mesh
    void DrawGPUTextureSkinnedMesh(const Ptr<CoreGraphics::Mesh>& mesh, IndexT primGroupIndex);
    /// draw a skinned mesh
    void DrawGPUTextureSkinnedMeshInstanced(const Ptr<CoreGraphics::Mesh>& mesh, IndexT primGroupIndex, SizeT numInstanced);
    
    /// allocate a row index in the joint texture
    IndexT AllocJointTextureRow();
    /// free a row index in the joint texture
    void FreeJointTextureRow(IndexT rowIndex);
    /// get a pointer to the joint texture row for the given character instance
    void* AcquireJointTextureRowPointer(const Ptr<Characters::CharacterInstance>& charInst, SizeT& outRowPitch);

    class DrawKey
    {
    private:
        void* ptr0;
        void* ptr1;

    public:
        /// default constructor
        DrawKey() : ptr0(0), ptr1(0) { };
        /// construct from character instance and mesh pointers
        DrawKey(Characters::CharacterInstance* c, CoreGraphics::Mesh* m) : ptr0(c), ptr1(m) { };            
        /// equality operator
        bool operator==(const DrawKey& rhs) const 
        { 
            return (this->ptr0 == rhs.ptr0) && (this->ptr1 == rhs.ptr1); 
        };
        /// inequality operator
        bool operator!=(const DrawKey& rhs) const 
        { 
            return (this->ptr0 != rhs.ptr0) || (this->ptr1 != rhs.ptr1); 
        };            
        /// greater operator
        bool operator>(const DrawKey& rhs) const 
        { 
            if (this->ptr0 == rhs.ptr0) return this->ptr1 > rhs.ptr1; 
            else return this->ptr0 > rhs.ptr0; 
        };                         
        /// lesser operator
        bool operator<(const DrawKey& rhs) const
        {
            if (this->ptr0 == rhs.ptr0) return this->ptr1 < rhs.ptr1;
            else return this->ptr0 < rhs.ptr0;
        };        
    };

    Util::Array<Characters::SkinnedMeshDrawInfo> drawInfos;   
    Util::Dictionary<DrawKey,IndexT> keyIndexMap;
    bool inFrame;
    bool isValid;
    bool inGather;
    bool inUpdateJointTexture;
};

//------------------------------------------------------------------------------
/**
*/
inline bool
SkinnedMeshRendererBase::IsValid() const
{
    return this->isValid;
}

//------------------------------------------------------------------------------
/**
    Override this method in a platform specific subclass!
*/
inline Characters::SkinningTechnique::Code
SkinnedMeshRendererBase::GetSkinningTechnique() const
{
    return Characters::SkinningTechnique::InvalidSkinningTechnique;
}

} // namespace Base
//------------------------------------------------------------------------------
