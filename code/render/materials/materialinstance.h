#pragma once
//------------------------------------------------------------------------------
/**
    @class Materials::MaterialInstance
    
    Describes an instance of a material, much like ShaderInstance
    
    (C) 2011-2013 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------

#include "core/refcounted.h"
#include "resources/resourceid.h"
#include "util/dictionary.h"
#include "materials/materialvariable.h"
#include "coregraphics/shaderinstance.h"
#include "models/modelnodetype.h"

namespace Frame
{
	class FramePassBase;
}

namespace Materials
{

class Material;
class MaterialInstance : public Core::RefCounted
{
	__DeclareClass(MaterialInstance);
public:
	/// constructor
	MaterialInstance();
	/// destructor
	virtual ~MaterialInstance();

	/// discard the material instance
	void Discard();
	/// checks if the object is valid
	bool IsValid() const;
	/// get pointer to the original material
	const Ptr<Material>& GetOriginalMaterial() const;

	/// applies material variables
	void Apply();

	/// returns true if the material instance has a variable by name
	bool HasVariableByName(const MaterialVariable::Name& n) const;
	/// get number of variables
	SizeT GetNumVariables() const;
	/// get variable by index
	const Ptr<MaterialVariable>& GetVariableByIndex(IndexT i) const;
	/// get variable by name
	const Ptr<MaterialVariable>& GetVariableByName(const MaterialVariable::Name& n) const;
	/// get shader instance by index
	const Ptr<CoreGraphics::ShaderInstance>& GetShaderInstanceByIndex(IndexT i) const;
	/// get shader instance by name
	const Ptr<CoreGraphics::ShaderInstance>& GetShaderInstanceByCode(const Models::ModelNodeType::Code& code);
	/// get number of shader instances
	const SizeT GetNumShaderInstances() const;

private:
	friend class Material;

	/// setup material instance from original material
	void Setup(const Ptr<Material>& origMaterial);
	/// discard the material instance
	void Cleanup();
	
	Util::Array<Ptr<CoreGraphics::ShaderInstance> > shaderInstances;
	Util::Dictionary<Models::ModelNodeType::Code, Ptr<CoreGraphics::ShaderInstance> > shaderInstancesByCode;
	Util::Array<Ptr<MaterialVariable> > materialVariables;
	Util::Dictionary<CoreGraphics::ShaderVariable::Name, Ptr<MaterialVariable> > materialVariablesByName;
	Ptr<Material> originalMaterial;
}; 

//------------------------------------------------------------------------------
/**
*/
inline const Ptr<Material>&
MaterialInstance::GetOriginalMaterial() const
{
	return this->originalMaterial;
}

//------------------------------------------------------------------------------
/**
*/
inline bool
MaterialInstance::HasVariableByName(const MaterialVariable::Name& n) const
{
	return this->materialVariablesByName.Contains(n);
}


//------------------------------------------------------------------------------
/**
*/
inline SizeT
MaterialInstance::GetNumVariables() const
{
	return this->materialVariables.Size();
}


//------------------------------------------------------------------------------
/**
*/
inline const Ptr<MaterialVariable>&
MaterialInstance::GetVariableByIndex(IndexT i) const
{
	return this->materialVariables[i];
}

} // namespace Materials
//------------------------------------------------------------------------------