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
	virtual void ApplyState();

	/// instantiate a shader variable by semantic and shader
	Ptr<Materials::MaterialVariableInstance> CreateMaterialVariableInstance(const Materials::MaterialVariable::Name& name);
	/// return true if a shader variable has been instantiated for the shader
	bool HasMaterialVariableInstance(const Materials::MaterialVariable::Name& name) const;
	/// returns true if material has variable
	bool HasMaterialVariable(const Materials::MaterialVariable::Name& name) const;
	/// get shader variable by shader
	const Ptr<Materials::MaterialVariableInstance>& GetMaterialVariableInstance(const Materials::MaterialVariable::Name& name) const;
	/// discard material instance
	void DiscardMaterialVariableInstance(Ptr<Materials::MaterialVariableInstance>& var);

protected:

	/// setup state node
	virtual void Setup(const Ptr<ModelInstance>& inst, const Ptr<ModelNode>& node, const Ptr<ModelNodeInstance>& parentNodeInst);
    /// called when removed from ModelInstance
    virtual void Discard();
	/// applies global variables
	virtual void ApplyGlobalVariables();

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

	Util::Dictionary<Materials::MaterialVariable::Name, Ptr<Materials::MaterialVariable> > globalVariables;
	Util::Dictionary<Materials::MaterialVariable::Name, Ptr<Materials::MaterialVariableInstance> > materialVariableInstances;
};


} // namespace Models
//------------------------------------------------------------------------------
