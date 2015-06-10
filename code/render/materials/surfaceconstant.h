#pragma once
//------------------------------------------------------------------------------
/**
	@class Materials::SurfaceConstant
	
	A surface constant is a value which represents a value constant to a specific type of surface.
    When rendering the same object using multiple shader, for example through shadow mapping, 
    deferred rendering and picking, we will want to have the ability to set a single constant
    which will be globally spread over all shaders. A surface constant is constant during
    the execution of the rendering, thus the name.
    
    An example for this is the alpha threshold when doing alpha clipping, 
    which is probably most intuitively seen when doing ordinary color shading, but is also
    relevant when clipping away pixels which should not be affected in the shadow map.

    SurfaceConstant needs a shader instance to apply to, so as to not apply to all
    shaders when only one is required.
	
	(C) 2015 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------
#include "core/refcounted.h"
#include "util/stringatom.h"
#include "util/variant.h"
#include "coregraphics/texture.h"
#include "resources/managedtexture.h"
namespace CoreGraphics
{
    class ShaderInstance;
    class ShaderVariable;
}

namespace Materials
{
class SurfaceConstantInstance;
class SurfaceConstant : public Core::RefCounted
{
	__DeclareClass(SurfaceConstant);
public:
	/// constructor
	SurfaceConstant();
	/// destructor
	virtual ~SurfaceConstant();

    /// create an instance of a constant
    Ptr<SurfaceConstantInstance> CreateInstance();

    /// set value of constant, the actual shader value will only be applied whenever explicitly called with the Apply function (which should happen when rendering)
    void SetValue(const Util::Variant& value);
    /// set value shorthand for textures
    void SetTexture(const Ptr<CoreGraphics::Texture>& tex);
    /// get value
    const Util::Variant& GetValue() const;

    /// applies this constant, which readies it for drawing, but only applies the value on one of the shaders
    void Apply(const Ptr<CoreGraphics::ShaderInstance>& shader);

private:
    friend class SurfaceConstantInstance;
    friend class SurfaceMaterial;

    /// setup constant, which initializes its name and bindings to its implementing shaders
    void Setup(const Util::StringAtom& name, const Util::Array<Ptr<CoreGraphics::ShaderInstance>>& variableToShaderMap);
    /// discard constant
    void Discard();

    Util::StringAtom name;
    Util::Variant value;
    Util::Array<Ptr<SurfaceConstantInstance>> instances;
    Util::Dictionary<Ptr<CoreGraphics::ShaderInstance>, Ptr<CoreGraphics::ShaderVariable>> variablesByShader;
};


//------------------------------------------------------------------------------
/**
*/
inline void
SurfaceConstant::SetValue(const Util::Variant& value)
{
    this->value = value;
}

//------------------------------------------------------------------------------
/**
*/
inline void
SurfaceConstant::SetTexture(const Ptr<CoreGraphics::Texture>& tex)
{
    this->value.SetObject(tex);
}

//------------------------------------------------------------------------------
/**
*/
inline const Util::Variant&
SurfaceConstant::GetValue() const
{
    return this->value;
}

} // namespace Materials