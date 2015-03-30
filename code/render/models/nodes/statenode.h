#pragma once
//------------------------------------------------------------------------------
/**
    @class Models::MaterialStateNode
    
    Model node that handles a material and its states for each shader
	it's rendered with. It inherits StateNode to avoid recreating the texture
	handling functions, but bypasses the StateNode tag-parsing. 
    
    (C) 2011-2013 Individual contributors, see AUTHORS file
*/
#include "models/nodes/transformnode.h"
#include "util/variant.h"
#include "resources/managedtexture.h"
#include "models/modelnodematerial.h"
#include "materials/materialinstance.h"
#include "materials/materialvariable.h"

namespace Materials
{
	class MaterialInstance;
}
//------------------------------------------------------------------------------
namespace Models
{
class StateNode : public TransformNode
{
	__DeclareClass(StateNode);
public:
	/// constructor
	StateNode();
	/// destructor
	virtual ~StateNode();

	/// set ModelNodeMaterial
	void SetMaterial(ModelNodeMaterial::Code t);
	/// get ModelNodeMaterial
	ModelNodeMaterial::Code GetMaterial() const;
	/// returns material as readable name
	const Util::String& GetMaterialName() const;
	/// create a model node instance
	virtual Ptr<ModelNodeInstance> CreateNodeInstance() const;
	/// parse data tag (called by loader code)
	virtual bool ParseDataTag(const Util::FourCC& fourCC, const Ptr<IO::BinaryReader>& reader);
	/// called when resources should be loaded
	void LoadResources(bool sync);
	/// called when resources should be unloaded
	void UnloadResources();
	/// apply state shared by all my ModelNodeInstances
	virtual void ApplySharedState(IndexT frameIndex);

	/// get shader instance by name
	const Ptr<Materials::MaterialInstance>& GetMaterialInstance() const;
	/// get shaderparams
	const Util::Array<Util::KeyValuePair<Util::StringAtom, Util::Variant> >& GetShaderParameter() const;

	/// gets a managed texture variable
	void SetManagedTextureVariable(const Util::StringAtom& varName, const Ptr<Resources::ManagedTexture>& texture);	
	/// returns managed resource by name (returns null pointer of no managed texture with that name exists)
	const Ptr<Resources::ManagedTexture> GetManagedTextureVariable(const Util::StringAtom& varName) const;	

	/// returns the resource state of the node, will only return true if all textures are loaded
	Resources::Resource::State GetResourceState() const;

protected:

	/// setup a new managed texture variable
	void SetupManagedTextureVariable(const Resources::ResourceId& texResId, const Ptr<Materials::MaterialVariable>& var, bool sync);
	/// update managed texture variables
	void UpdateManagedTextureVariables(IndexT frameIndex);

	class ManagedTextureVariable
	{
	public:
		/// default constructor
		ManagedTextureVariable() {};
		/// constructor
		ManagedTextureVariable(const Ptr<Resources::ManagedTexture>& tex, const Ptr<Materials::MaterialVariable>& var) :
			managedTexture(tex),
			materialVariable(var)
		{ };

		Ptr<Resources::ManagedTexture> managedTexture;
		Ptr<Materials::MaterialVariable> materialVariable;
	};

	Resources::Resource::State stateLoaded;
	Util::String materialName;
	ModelNodeMaterial::Code materialCode;
	Ptr<Materials::MaterialInstance> materialInstance;
	Util::Array<Util::KeyValuePair<Util::StringAtom, Util::Variant> > shaderParams;
	Util::Array<ManagedTextureVariable> managedTextureVariables;	
}; 



//------------------------------------------------------------------------------
/**
*/
inline void 
StateNode::SetMaterial( ModelNodeMaterial::Code t )
{
	this->materialCode = t;
	this->type = t;
	this->materialName = ModelNodeMaterial::ToName(t).AsString();
}

//------------------------------------------------------------------------------
/**
*/
inline ModelNodeMaterial::Code 
StateNode::GetMaterial() const
{
	return this->materialCode;
}

//------------------------------------------------------------------------------
/**
*/
inline const Ptr<Materials::MaterialInstance>& 
StateNode::GetMaterialInstance() const
{
	n_assert(0 != this->materialInstance);
	return this->materialInstance;
}


//------------------------------------------------------------------------------
/**
*/
inline const Util::String& 
StateNode::GetMaterialName() const
{
	return this->materialName;
}


} // namespace Models
//------------------------------------------------------------------------------