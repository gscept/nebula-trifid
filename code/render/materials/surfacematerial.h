#pragma once
//------------------------------------------------------------------------------
/**
	@class Materials::SurfaceMaterial
	
	Material instance which is apply able on a single model node instance.
    This resource uses a material as template (shaders, passes) but implements its own variables.
    It is similar to how the old material instance worked, except it is exchangeable, and created as a resource.

    A surface material thus represents all shader constants present in all the shaders
    its material template is implementing. This makes a surface material reusable and shareable
    over several models, and also exchangeable without having to modify the model resource.

    The SurfaceMaterial class is not responsible for applying the shaders themselves, this is
    handled by the FrameBatch, but each state node instance should have a surface material.
    If a surface material fails to load, it is assigned the placeholder material instead.

    Surfaces are cloneable, which makes it possible to have per-instance unique materials.
    A clone will copy the original surface, but will also allow for constants to be set
    on a per-object basis. Since SurfaceMaterial is a resource, it is only ever truly destroyed
    once all instances of the material (and its clones) gets discarded.

    However surface constants are also instanciateable, which means one can, 
    without duplicating a material, have a variable which overrides the original.
    Cloning a material is useful if one wishes to make a entirely new material
    from an existing one, without the overhead of having per-instance constant
    overrides.
	
	(C) 2015 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------
#include "resources/resource.h"
#include "material.h"
#include "physics/materialtable.h"
#include "materials/materialtype.h"
#include "surfaceconstant.h"
namespace Materials
{
class SurfaceMaterial : public Resources::Resource
{
	__DeclareClass(SurfaceMaterial);
public:
	/// constructor
	SurfaceMaterial();
	/// destructor
	virtual ~SurfaceMaterial();

    /// discard surface
    void Discard();
    /// unload surface (overrides the one in resource)
    void Unload();

    /// get original material
    const Ptr<Materials::Material>& GetMaterialTemplate();
    /// apply surface based on pass
    const Ptr<CoreGraphics::ShaderInstance>& GetShaderInstance(const Frame::BatchGroup::Code& pass);
    /// set pre-defined value of a parameter, submit the true as the last argument if the material should change immediately
    void SetValue(const Util::StringAtom& param, const Util::Variant& value);
    /// set texture directly, shorthand for setvalue with a texture
    void SetTexture(const Util::StringAtom& param, const Ptr<CoreGraphics::Texture>& tex);
    /// set a managed texture (which may load asynchronously), which is then retrieved each frame
    void SetTexture(const Util::StringAtom& param, const Ptr<Resources::ManagedTexture>& tex);

    /// returns true if surface has a constant with the given name
    bool HasConstant(const Util::StringAtom& name) const;
    /// return pointer to surface constant depending on name
    const Ptr<SurfaceConstant>& GetConstant(const Util::StringAtom& name) const;

    /// get material type (from original material)
    const MaterialType::Code& GetMaterialType() const;

    /// apply the surface variables, but only for a specific shader instance
    void Apply(const Ptr<CoreGraphics::ShaderInstance>& shader);

    /// clone the surface material
    Ptr<SurfaceMaterial> Clone() const;

private:
    friend class StreamSurfaceMaterialLoader;
	friend class StreamSurfaceMaterialSaver;

    /// setup surface from original material
    void Setup(const Ptr<Material>& material);

    struct DeferredTextureBinding
    {
    public:
        Ptr<Resources::ManagedTexture> tex;
        Util::StringAtom var;
    };

    Util::Array<Ptr<SurfaceConstant>> constants;
    Util::Dictionary<Util::StringAtom, Ptr<SurfaceConstant>> constantsByName;
    Util::Dictionary<Util::StringAtom, Util::Variant> staticValues;
    Util::Array<DeferredTextureBinding> managedTextures;
    Ptr<Material> materialTemplate;
};

//------------------------------------------------------------------------------
/**
*/
inline const Ptr<Materials::Material>&
SurfaceMaterial::GetMaterialTemplate()
{
    return this->materialTemplate;
}

//------------------------------------------------------------------------------
/**
*/
inline const Ptr<SurfaceConstant>&
SurfaceMaterial::GetConstant(const Util::StringAtom& name) const
{
    return this->constantsByName[name];
}

//------------------------------------------------------------------------------
/**
*/
inline bool
SurfaceMaterial::HasConstant(const Util::StringAtom& name) const
{
    return this->constantsByName.Contains(name);
}

//------------------------------------------------------------------------------
/**
*/
inline const MaterialType::Code&
SurfaceMaterial::GetMaterialType() const
{
    return this->materialTemplate->GetCode();
}

} // namespace Materials