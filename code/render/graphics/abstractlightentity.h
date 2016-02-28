#pragma once
//------------------------------------------------------------------------------
/**
    @class Graphics::AbstractLightEntity
    
    Base class for light sources. Light sources do not directly influence
    the render pipeline (like manipulating shader variables, etc...). This
    will be handled by the LightServer and ShadowServer singletons which
    may implement platform specific lighting models.
    
    (C) 2007 Radon Labs GmbH
    (C) 2013-2016 Individual contributors, see AUTHORS file
*/
#include "graphics/graphicsentity.h"
#include "lighting/lighttype.h"
#include "resources/resourceid.h"
#include "resources/managedtexture.h"

//------------------------------------------------------------------------------
namespace Graphics
{
class AbstractLightEntity : public Graphics::GraphicsEntity
{
    __DeclareClass(AbstractLightEntity);
public:
    /// constructor
    AbstractLightEntity();
    /// destructor
    virtual ~AbstractLightEntity();

    /// get the light type
    Lighting::LightType::Code GetLightType() const;
    /// set primary light color
    void SetColor(const Math::float4& c);
    /// get primary light color
    const Math::float4& GetColor() const;
    /// enable/disable shadow casting
    void SetCastShadows(bool b);
    /// get shadow casting flag
    bool GetCastShadows() const;   
    /// set projection map UV offset and scale (xy->offset, zw->scale)
    void SetProjMapUvOffsetAndScale(const Math::float4& v);
    /// get projection map UV offset and scale
    const Math::float4& GetProjMapUvOffsetAndScale() const;

    /// get inverse transform (transforms from world to light space)
    const Math::matrix44& GetInvTransform() const;
    /// get light-projection matrix (transforms from light space to light projection space)
    const Math::matrix44& GetProjTransform() const;
    /// get world-to-light-projection transform (transform from world to light projection space)
    const Math::matrix44& GetInvLightProjTransform() const;

    /// set shadow buffer uv rectangle (optionally set by light/shadow servers)
    void SetShadowBufferUvOffsetAndScale(const Math::float4& uvOffset);
    /// get shadow buffer uv rectangle
    const Math::float4& GetShadowBufferUvOffsetAndScale() const;

	/// 'touches' the light resource, which updates the projection map if it has changed
	void TouchProjectionTexture();
	/// sets light projection texture resource
	void SetProjectionTexture(const Resources::ResourceId& res);
	/// return light projection map
	const Ptr<Resources::ManagedTexture>& GetProjectionTexture() const;

    /// get cast shadows this frame
    bool GetCastShadowsThisFrame() const;
	/// set the frequency with which we update the shadow maps for this light (each N'th frame)
	void SetShadowCastingFrequency(int freq);
    /// set shadow transform
    void SetShadowTransform(const Math::matrix44& val);
	/// get shadow transform
	const Math::matrix44& GetShadowTransform();
    /// get shadow transform
    const Math::matrix44& GetShadowInvTransform();  
    /// get shadow projection transform
    const Math::matrix44& GetShadowInvLightProjTransform();  
    /// get shadow projection transform
    const Math::matrix44& GetShadowProjTransform();    
    /// get ShadowIntensity	
    float GetShadowIntensity() const;
    /// set ShadowIntensity
    void SetShadowIntensity(float val);
	/// get shadow bias
	float GetShadowBias() const;
	/// set shadow bias
	void SetShadowBias(float val);
	/// get if light is volumetric
	bool GetVolumetric() const;
	/// set volumetric light
	void SetVolumetric(bool b);
	/// get volumetric scale
	float GetVolumetricScale() const;
	/// set volumetric scale
	void SetVolumetricScale(float f);
	/// get volumetric intensity
	float GetVolumetricIntensity() const;
	/// set volumetric intensity
	void SetVolumetricIntensity(float f);
	
   
    /// handle a message
    virtual void HandleMessage(const Ptr<Messaging::Message>& msg);

protected:
    /// called from view
	virtual void OnResolveVisibility(IndexT frameIndex, bool updateLod = false);
    /// called when transform matrix changed
    virtual void OnTransformChanged();
    /// set the light type (must be called from sub-classes constructor
    void SetLightType(Lighting::LightType::Code c);    
    /// called to render a debug visualization of the entity
    virtual void OnRenderDebug();
    /// update shadow transforms
    void UpdateShadowTransforms();

	/// notify the light there is a new frame, this will cause the CastShadowsThisFrame to update
	void UpdateFrame();

    Lighting::LightType::Code lightType;
    Math::matrix44 invTransform;
    Math::matrix44 projTransform;
    Math::matrix44 invLightProjTransform;        
    Math::float4 color;
    Math::float4 projMapUvOffsetAndScale;
    Math::float4 shadowBufferUvOffsetAndScale;
	bool volumetric;
	float volumetricScale;
	float volumetricIntensity;
    bool castShadows;
    bool castShadowsThisFrame; 
	int shadowCastingFrequency;
	int shadowCastingFrame;
    float shadowIntensity;
	float shadowBias;
    Math::matrix44 shadowTransform;
    Math::matrix44 shadowInvTransform;   
    Math::matrix44 shadowInvLightProjTransform;   
    Math::matrix44 shadowProjTransform;
	Resources::ResourceId projectionTextureId;
	Ptr<Resources::ManagedTexture> projectionTexture;
    bool shadowTransformsDirty;
};

//------------------------------------------------------------------------------
/**
*/
inline void
AbstractLightEntity::SetLightType(Lighting::LightType::Code c)
{
    this->lightType = c;
}

//------------------------------------------------------------------------------
/**
*/
inline Lighting::LightType::Code
AbstractLightEntity::GetLightType() const
{
    return this->lightType;
}

//------------------------------------------------------------------------------
/**
*/
inline void
AbstractLightEntity::SetColor(const Math::float4& c)
{
    this->color = c;
}

//------------------------------------------------------------------------------
/**
*/
inline const Math::float4&
AbstractLightEntity::GetColor() const
{
    return this->color;
}

//------------------------------------------------------------------------------
/**
*/
inline void
AbstractLightEntity::SetProjMapUvOffsetAndScale(const Math::float4& v)
{
    this->projMapUvOffsetAndScale = v;
}

//------------------------------------------------------------------------------
/**
*/
inline const Math::float4&
AbstractLightEntity::GetProjMapUvOffsetAndScale() const
{
    return this->projMapUvOffsetAndScale;
}

//------------------------------------------------------------------------------
/**
*/
inline void
AbstractLightEntity::SetShadowBufferUvOffsetAndScale(const Math::float4& v)
{
    this->shadowBufferUvOffsetAndScale = v;
}

//------------------------------------------------------------------------------
/**
*/
inline const Math::float4&
AbstractLightEntity::GetShadowBufferUvOffsetAndScale() const
{
    return this->shadowBufferUvOffsetAndScale;
}

//------------------------------------------------------------------------------
/**
*/
inline void 
AbstractLightEntity::SetProjectionTexture( const Resources::ResourceId& res )
{
	this->projectionTextureId = res;
}

//------------------------------------------------------------------------------
/**
*/
inline const Ptr<Resources::ManagedTexture>& 
AbstractLightEntity::GetProjectionTexture() const
{
	return this->projectionTexture;
}

//------------------------------------------------------------------------------
/**
*/
inline void
AbstractLightEntity::SetCastShadows(bool b)
{
    this->castShadows = b;
}

//------------------------------------------------------------------------------
/**
*/
inline bool
AbstractLightEntity::GetCastShadows() const
{
    return this->castShadows;
}

//------------------------------------------------------------------------------
/**
*/
inline const Math::matrix44&
AbstractLightEntity::GetInvTransform() const
{
    return this->invTransform;
}

//------------------------------------------------------------------------------
/**
*/
inline const Math::matrix44&
AbstractLightEntity::GetProjTransform() const
{
    return this->projTransform;
}

//------------------------------------------------------------------------------
/**
*/
inline const Math::matrix44&
AbstractLightEntity::GetInvLightProjTransform() const
{
    return this->invLightProjTransform;
}       

//------------------------------------------------------------------------------
/**
*/
inline bool 
AbstractLightEntity::GetCastShadowsThisFrame() const
{
    return this->castShadowsThisFrame;
}

//------------------------------------------------------------------------------
/**
*/
inline void
AbstractLightEntity::SetShadowCastingFrequency(int freq)
{
	this->shadowCastingFrequency = freq;
}

//------------------------------------------------------------------------------
/**
*/
inline void 
AbstractLightEntity::SetShadowTransform(const Math::matrix44& val)
{
    this->shadowTransform = val;
    this->shadowTransformsDirty = true;
}   

//------------------------------------------------------------------------------
/**
*/
inline float 
AbstractLightEntity::GetShadowIntensity() const
{
    return this->shadowIntensity;
}

//------------------------------------------------------------------------------
/**
*/
inline void 
AbstractLightEntity::SetShadowIntensity(float val)
{
    this->shadowIntensity = val;
}

//------------------------------------------------------------------------------
/**
*/
inline float 
AbstractLightEntity::GetShadowBias() const
{
	return this->shadowBias;
}

//------------------------------------------------------------------------------
/**
*/
inline void 
AbstractLightEntity::SetShadowBias( float val )
{
	this->shadowBias = val;
}

//------------------------------------------------------------------------------
/**
*/
inline bool 
AbstractLightEntity::GetVolumetric() const
{
	return this->volumetric;
}

//------------------------------------------------------------------------------
/**
*/
inline float 
AbstractLightEntity::GetVolumetricScale() const
{
	return this->volumetricScale;
}

//------------------------------------------------------------------------------
/**
*/
inline void 
AbstractLightEntity::SetVolumetricScale( float f )
{
	this->volumetricScale = f;
}

//------------------------------------------------------------------------------
/**
*/
inline float 
AbstractLightEntity::GetVolumetricIntensity() const
{
	return this->volumetricIntensity;
}

//------------------------------------------------------------------------------
/**
*/
inline void 
AbstractLightEntity::SetVolumetricIntensity( float f )
{
	this->volumetricIntensity = f;
}

} // namespace Graphics
//------------------------------------------------------------------------------

    