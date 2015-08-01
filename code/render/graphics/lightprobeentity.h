#pragma once
//------------------------------------------------------------------------------
/**
	@class Graphics::LightProbeEntity
	
	A light probe entity is used to render reflections and irradiance to a local area.
	
	(C) 2012-2014 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------
#include "graphicsentity.h"
#include "lighting/environmentprobe.h"
#include "coregraphics/shaderinstance.h"
namespace Graphics
{
class LightProbeEntity : public GraphicsEntity
{
	__DeclareClass(LightProbeEntity);
public:

	enum LightProbeShapeType
	{
		Sphere,
		Box,

		NumProbeShapeTypes
	};

	/// constructor
	LightProbeEntity();
	/// destructor
	virtual ~LightProbeEntity();

	/// compute clip status against bounding box
	virtual Math::ClipStatus::Type ComputeClipStatus(const Math::bbox& box);

    /// handle being attached to the scene
    void OnActivate();
    /// handle being detached from the scene
    void OnDeactivate();

    /// apply probe to entity
    void ApplyProbe(const Ptr<Lighting::EnvironmentProbe>& probe);

	/// set layer
	void SetLayer(int layer);
	/// get layer
	const int GetLayer() const;
	/// set falloff
	void SetFalloff(float falloff);
	/// get falloff
	const float GetFalloff() const;
	/// set power
	void SetPower(float power);
	/// get power
	const float GetPower() const;
	/// set zone
	void SetZone(const Math::bbox& box);
	/// get zone
	const Math::bbox& GetZone() const;
	/// set if this light probe should perform parallax correctness
	void SetParallaxCorrected(bool b);
	/// returns true if this probe performs parallax correction
	const bool GetParallaxCorrected() const;
	
    /// get shader
    const Ptr<CoreGraphics::ShaderInstance>& GetShaderInstance() const;

	/// set shape type
	void SetShapeType(LightProbeShapeType shape);
	/// get shape type
	const LightProbeShapeType GetShapeType() const;

	/// get environment probe
	void SetEnvironmentProbe(const Ptr<Lighting::EnvironmentProbe>& probe);
	/// get environment probe
	const Ptr<Lighting::EnvironmentProbe>& GetEnvironmentProbe() const;

protected:
	/// called from view when visible
	virtual void OnResolveVisibility(IndexT frameIndex, bool updateLod = false);

private:
	int layer;
    bool isDirty;
	bool parallaxCorrected;
	Math::scalar falloff;
	Math::scalar power;
	Math::bbox zone;
	LightProbeShapeType shape;
	Ptr<Lighting::EnvironmentProbe> probe;

    Ptr<CoreGraphics::ShaderInstance> shader;
    Ptr<CoreGraphics::ShaderVariableInstance> lightProbeReflectionVar;
    Ptr<CoreGraphics::ShaderVariableInstance> lightProbeIrradianceVar;
    Ptr<CoreGraphics::ShaderVariableInstance> lightProbeFalloffVar;
    Ptr<CoreGraphics::ShaderVariableInstance> lightProbePowerVar;
    Ptr<CoreGraphics::ShaderVariableInstance> lightProbeReflectionNumMipsVar;
    Ptr<CoreGraphics::ShaderVariableInstance> lightProbeBboxMinVar;
    Ptr<CoreGraphics::ShaderVariableInstance> lightProbeBboxMaxVar;
    Ptr<CoreGraphics::ShaderVariableInstance> lightProbeBboxCenterVar;
    Ptr<CoreGraphics::ShaderVariableInstance> lightProbeTransformVar;
};


//------------------------------------------------------------------------------
/**
*/
inline void
LightProbeEntity::SetLayer(int layer)
{
	this->layer = layer;
    this->isDirty = true;
}

//------------------------------------------------------------------------------
/**
*/
inline const int
LightProbeEntity::GetLayer() const
{
	return this->layer;
}

//------------------------------------------------------------------------------
/**
*/
inline void
LightProbeEntity::SetFalloff(float falloff)
{
	this->falloff = falloff;
    this->isDirty = true;
}

//------------------------------------------------------------------------------
/**
*/
inline const float
LightProbeEntity::GetFalloff() const
{
	return this->falloff;
}

//------------------------------------------------------------------------------
/**
*/
inline void
LightProbeEntity::SetPower(float power)
{
	this->power = power;
    this->isDirty = true;
}

//------------------------------------------------------------------------------
/**
*/
inline const float
LightProbeEntity::GetPower() const
{
	return this->power;
}

//------------------------------------------------------------------------------
/**
*/
inline void
LightProbeEntity::SetZone(const Math::bbox& box)
{
	this->zone = box;
    this->isDirty = true;
}

//------------------------------------------------------------------------------
/**
*/
inline const Math::bbox&
LightProbeEntity::GetZone() const
{
	return this->zone;
}

//------------------------------------------------------------------------------
/**
*/
inline void
LightProbeEntity::SetParallaxCorrected(bool b)
{
	this->parallaxCorrected = b;
    this->isDirty = true;
}

//------------------------------------------------------------------------------
/**
*/
inline const bool
LightProbeEntity::GetParallaxCorrected() const
{
	return this->parallaxCorrected;
}

//------------------------------------------------------------------------------
/**
*/
inline void
LightProbeEntity::SetShapeType(LightProbeShapeType shape)
{
	this->shape = shape;
    this->isDirty = true;
}

//------------------------------------------------------------------------------
/**
*/
inline const LightProbeEntity::LightProbeShapeType
LightProbeEntity::GetShapeType() const
{
	return this->shape;
}

//------------------------------------------------------------------------------
/**
*/
inline void
LightProbeEntity::SetEnvironmentProbe(const Ptr<Lighting::EnvironmentProbe>& probe)
{
	this->probe = probe;
}

//------------------------------------------------------------------------------
/**
*/
inline const Ptr<Lighting::EnvironmentProbe>&
LightProbeEntity::GetEnvironmentProbe() const
{
	return this->probe;
}

//------------------------------------------------------------------------------
/**
*/
inline const Ptr<CoreGraphics::ShaderInstance>&
LightProbeEntity::GetShaderInstance() const
{
    return this->shader;
}

} // namespace Graphics