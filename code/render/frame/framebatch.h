#pragma once
//------------------------------------------------------------------------------
/**
    @class Frame::FrameBatch
    
    A frame batch encapsulates the rendering of a batch of ModelNodeInstances.
    Frame batches renders objects based on their materials
    
    (C) 2007 Radon Labs GmbH
    (C) 2013-2015 Individual contributors, see AUTHORS file
*/
#include "core/refcounted.h"
#include "materials/materialinstance.h"
#include "coregraphics/shaderinstance.h"
#include "coregraphics/shadervariableinstance.h"
#include "coregraphics/framebatchtype.h"
#include "coregraphics/shaderfeature.h"
#include "frame/batchgroup.h"
#include "frame/lightingmode.h"
#include "frame/sortingmode.h"
#include "debug/debugtimer.h"

#define NEBULA3_FRAME_LOG_ENABLED   (0)
#if NEBULA3_FRAME_LOG_ENABLED
#define FRAME_LOG(...) n_printf(__VA_ARGS__); n_printf("\n")
#else
#define FRAME_LOG(...)
#endif

namespace Materials
{
	class MaterialTemplate;
}

namespace Models
{
	class ModelNodeInstance;
}

//------------------------------------------------------------------------------
namespace Frame
{
class FrameBatch : public Core::RefCounted
{
    __DeclareClass(FrameBatch);
public:
    /// constructor
    FrameBatch();
    /// destructor
    virtual ~FrameBatch();
    /// discard the frame batch
    void Discard();
    /// render the batch
    void Render();

	/// set material
	void SetMaterial(const Util::String& mat);
	/// get material
	const Util::String& GetMaterial() const;
    /// set batch type
    void SetType(CoreGraphics::FrameBatchType::Code t);
    /// get batch type
    CoreGraphics::FrameBatchType::Code GetType() const;
    /// set model node filter
    void SetBatchGroup(const Frame::BatchGroup::Code& b);
    /// get model node filter
    const Frame::BatchGroup::Code& GetBatchGroup() const;
    /// set lighting mode
    void SetLightingMode(LightingMode::Code c);
    /// get lighting mode
    LightingMode::Code GetLightingMode() const;
    /// set sorting mode
    void SetSortingMode(SortingMode::Code c);
    /// get sorting mode
    SortingMode::Code GetSortingMode() const;
    /// set shader features
    void SetShaderFeatures(CoreGraphics::ShaderFeature::Mask m);
    /// get shader features
    CoreGraphics::ShaderFeature::Mask GetShaderFeatures() const;

    /// set if this batch should force every node instance drawn in it to become instanced
    void SetForceInstancing(bool b, SizeT numinstances);
    /// return if this batch forces instancing
    bool GetForceInstancing() const;
    /// return number of instances
    const SizeT GetNumInstances() const;

    /// add a shader variable instance to the frame batch
	void AddVariable(const Util::StringAtom& semantic, const Ptr<CoreGraphics::ShaderVariableInstance>& var);
    /// get number of shader variables
    SizeT GetNumVariables() const;
    /// get shader variable by index
    const Ptr<CoreGraphics::ShaderVariableInstance>& GetVariableByIndex(IndexT i) const;
	/// get shader variable by name
	const Ptr<CoreGraphics::ShaderVariableInstance>& GetVariableByName(const Util::StringAtom& name) const;

#if NEBULA3_ENABLE_PROFILING
    /// add batch profiler
    void SetBatchDebugTimer(const Util::String& name);
#endif

private:
    /// actual rendering method
    void RenderBatch();
	/// sort node instances based on sorting mode
	void Sort(Util::Array<Ptr<Models::ModelNodeInstance>>& instances);

    bool forceInstancing;
    SizeT instancingCount;
	Util::String material;
    CoreGraphics::FrameBatchType::Code batchType;
    Frame::BatchGroup::Code batchGroup;
    LightingMode::Code lightingMode;
    SortingMode::Code sortingMode;
    CoreGraphics::ShaderFeature::Mask shaderFeatures;
    Util::Array<Ptr<CoreGraphics::ShaderVariableInstance> > shaderVariables;
	Util::Dictionary<Util::StringAtom, Ptr<CoreGraphics::ShaderVariableInstance> > shaderVariablesByName;

    _declare_timer(debugTimer);
};

//------------------------------------------------------------------------------
/**
*/
inline void
FrameBatch::SetType(CoreGraphics::FrameBatchType::Code t)
{
    this->batchType = t;
}

//------------------------------------------------------------------------------
/**
*/
inline CoreGraphics::FrameBatchType::Code
FrameBatch::GetType() const
{
    return this->batchType;
}

//------------------------------------------------------------------------------
/**
*/
inline void
FrameBatch::SetMaterial(const Util::String& mat)
{
	this->material = mat;
}

//------------------------------------------------------------------------------
/**
*/
inline const Util::String&
FrameBatch::GetMaterial() const
{
	return this->material;
}

//------------------------------------------------------------------------------
/**
*/
inline void
FrameBatch::SetBatchGroup(const Frame::BatchGroup::Code& b)
{
    this->batchGroup = b;
}

//------------------------------------------------------------------------------
/**
*/
inline const Frame::BatchGroup::Code&
FrameBatch::GetBatchGroup() const
{
    return this->batchGroup;
}

//------------------------------------------------------------------------------
/**
*/
inline void
FrameBatch::SetLightingMode(LightingMode::Code c)
{
    this->lightingMode = c;
}

//------------------------------------------------------------------------------
/**
*/
inline LightingMode::Code
FrameBatch::GetLightingMode() const
{
    return this->lightingMode;
}

//------------------------------------------------------------------------------
/**
*/
inline void
FrameBatch::SetSortingMode(SortingMode::Code c)
{
    this->sortingMode = c;
}

//------------------------------------------------------------------------------
/**
*/
inline SortingMode::Code
FrameBatch::GetSortingMode() const
{
    return this->sortingMode;
}

//------------------------------------------------------------------------------
/**
*/
inline void
FrameBatch::AddVariable(const Util::StringAtom& semantic, const Ptr<CoreGraphics::ShaderVariableInstance>& var)
{
	n_assert(!this->shaderVariablesByName.Contains(semantic));
    this->shaderVariables.Append(var);
	this->shaderVariablesByName.Add(semantic, var);
}

//------------------------------------------------------------------------------
/**
*/
inline SizeT
FrameBatch::GetNumVariables() const
{
    return this->shaderVariables.Size();
}

//------------------------------------------------------------------------------
/**
*/
inline const Ptr<CoreGraphics::ShaderVariableInstance>&
FrameBatch::GetVariableByIndex(IndexT i) const
{
    return this->shaderVariables[i];
}

//------------------------------------------------------------------------------
/**
*/
inline const Ptr<CoreGraphics::ShaderVariableInstance>& 
FrameBatch::GetVariableByName( const Util::StringAtom& name ) const
{
	n_assert(this->shaderVariablesByName.Contains(name));
	return this->shaderVariablesByName[name];
}

//------------------------------------------------------------------------------
/**
*/
inline void
FrameBatch::SetShaderFeatures(CoreGraphics::ShaderFeature::Mask m)
{
    this->shaderFeatures = m;
}

//------------------------------------------------------------------------------
/**
*/
inline CoreGraphics::ShaderFeature::Mask
FrameBatch::GetShaderFeatures() const
{
    return this->shaderFeatures;
}

//------------------------------------------------------------------------------
/**
*/
inline void 
FrameBatch::SetForceInstancing( bool b, SizeT numinstances )
{
    this->forceInstancing = b;
    this->instancingCount = numinstances;
}

//------------------------------------------------------------------------------
/**
*/
inline bool 
FrameBatch::GetForceInstancing() const
{
    return this->forceInstancing;
}

//------------------------------------------------------------------------------
/**
*/
inline const SizeT 
FrameBatch::GetNumInstances() const
{
    return this->instancingCount;
}

} // namespace Frame
//------------------------------------------------------------------------------

