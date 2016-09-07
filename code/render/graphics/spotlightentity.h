#pragma once
//------------------------------------------------------------------------------
/**
    @class Graphics::SpotLightEntity
    
    Implements a local spot light. The spot light's cone is computed
    from the transformation matrix of the light entity. The spot light
    cone will point along -z, and the cone's angle will be defined
    by the length of x and y component at the end of the z component.
    
    (C) 2007 Radon Labs GmbH
    (C) 2013-2015 Individual contributors, see AUTHORS file
*/
#include "graphics/abstractlightentity.h"

//------------------------------------------------------------------------------
namespace CoreGraphics
{
	class ShaderState;
	class ShaderVariable;
};

namespace Graphics
{
class SpotLightEntity : public AbstractLightEntity
{
    __DeclareClass(SpotLightEntity);
public:
    /// constructor
    SpotLightEntity();
	/// destructor
	virtual ~SpotLightEntity();
    /// compute clip status against bounding box
    virtual Math::ClipStatus::Type ComputeClipStatus(const Math::bbox& box);
    /// handle a message
    virtual void HandleMessage(const Ptr<Messaging::Message>& msg);

    /// set transform using position, direction, range and cone angle
    void SetTransformFromPosDirRangeAndCone(const Math::point& pos, const Math::vector& dir, float range, float coneAngle);

	/// get shader state
	const Ptr<CoreGraphics::ShaderState>& GetShaderState() const;

protected:
    /// called when transform matrix changed
    virtual void OnTransformChanged();
    /// called to render a debug visualization of the entity
    virtual void OnRenderDebug();
	/// called when entity is created
	virtual void OnActivate();
	/// called before entity is destroyed
	virtual void OnDeactivate();

	Ptr<CoreGraphics::ShaderState> shaderState;
	Ptr<CoreGraphics::ShaderVariable> posRangeVar;
	Ptr<CoreGraphics::ShaderVariable> colorVar;
	Ptr<CoreGraphics::ShaderVariable> transformVar;
	Ptr<CoreGraphics::ShaderVariable> texProjVar;
	Ptr<CoreGraphics::ShaderVariable> texVar;

	Ptr<CoreGraphics::ShaderVariable> shadowProjVar;
	Ptr<CoreGraphics::ShaderVariable> shadowIntensityVar;
	Ptr<CoreGraphics::ShaderVariable> shadowOffsetVar;
};

//------------------------------------------------------------------------------
/**
*/
inline const Ptr<CoreGraphics::ShaderState>&
SpotLightEntity::GetShaderState() const
{
	return this->shaderState;
}

} // namespace Graphics
//------------------------------------------------------------------------------
