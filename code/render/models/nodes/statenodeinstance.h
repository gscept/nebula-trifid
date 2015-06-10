#pragma once
//------------------------------------------------------------------------------
/**
    @class Models::MaterialStateNodeInstance

    Holds and applies per-instance shader states for models with materials attached
    
    (C) 2011-2013 Individual contributors, see AUTHORS file
*/
#include "models/nodes/transformnodeinstance.h"
#include "materials/materialvariable.h"
#include "coregraphics/shaderbuffer.h"
#include "materials/surfacematerial.h"
#include "coregraphics/shadersemantics.h"

//#define STATE_NODE_USE_PER_OBJECT_BUFFER
//------------------------------------------------------------------------------
namespace Models
{
class StateNodeInstance : public TransformNodeInstance
{
    __DeclareClass(StateNodeInstance);
public:
    /// constructor
    StateNodeInstance();
    /// destructor
    virtual ~StateNodeInstance();

	/// apply per-instance state for a shader prior to rendering
    virtual void ApplyState(const Ptr<CoreGraphics::ShaderInstance>& shader);

    /// set surface material on node, be careful to clear up any references to SurfaceConstantInstances if this is done
    void SetMaterial(const Ptr<Materials::SurfaceMaterial>& material);
    /// get surface on node
    const Ptr<Materials::SurfaceMaterial>& GetMaterial() const;

    /// create a new surface constant instance by using a name
    Ptr<Materials::SurfaceConstantInstance> CreateSurfaceConstantInstance(const Util::StringAtom& name);
    /// returns true if we already created a surface constant instance with the given name
    bool HasSurfaceConstantInstance(const Util::StringAtom& name);
    /// returns reference to surface constant instance, asserts it exists
    const Ptr<Materials::SurfaceConstantInstance>& GetSurfaceConstantInstance(const Util::StringAtom& name);
    /// discard a surface constant instance if it exists
    void DiscardSurfaceConstantInstance(const Ptr<Materials::SurfaceConstantInstance>& var);

protected:

	/// setup state node
	virtual void Setup(const Ptr<ModelInstance>& inst, const Ptr<ModelNode>& node, const Ptr<ModelNodeInstance>& parentNodeInst);
    /// called when removed from ModelInstance
    virtual void Discard();
	/// applies global variables
	virtual void ApplySharedVariables();

#ifdef STATE_NODE_USE_PER_OBJECT_BUFFER
	struct PerObject
	{
		Math::matrix44 model;
		Math::matrix44 invModel;
		Math::matrix44 mvp;
		Math::matrix44 modelView;
		int objectId;
	} perObject;	
	Ptr<CoreGraphics::ShaderBuffer> perObjectBuffer;
#endif

    Ptr<Materials::SurfaceMaterial> material;
    Materials::MaterialType::Code materialCode;
    Util::Dictionary<Util::StringAtom, Ptr<Materials::SurfaceConstant>> sharedConstants;
    Util::Dictionary<Util::StringAtom, Ptr<Materials::SurfaceConstantInstance>> surfaceConstantInstanceByName;
};


//------------------------------------------------------------------------------
/**
*/

//------------------------------------------------------------------------------
/**
*/
inline const Ptr<Materials::SurfaceMaterial>&
StateNodeInstance::GetMaterial() const
{
    return this->material;
}

} // namespace Models
//------------------------------------------------------------------------------
