#pragma once
//------------------------------------------------------------------------------
/**
    @class Godrays::GodrayRenderModule
    
    Client-side interface for godray rendering
    
    (C) 2013 Gustav Sterbrant
*/
//------------------------------------------------------------------------------
#include "rendermodules/rendermodule.h"
#include "core/singleton.h"
#include "graphics/modelentity.h"
#include "graphics/stage.h"
namespace Godrays
{
class GodrayRenderModule : public RenderModules::RenderModule
{
	__DeclareClass(GodrayRenderModule);
	__DeclareSingleton(GodrayRenderModule);
public:
	/// constructor
	GodrayRenderModule();
	/// destructor
	virtual ~GodrayRenderModule();

	/// setup godrays
	virtual void Setup();
	/// discard void rays
	virtual void Discard();

	/// sets the sun texture resource
	void SetSunTexture(const Resources::ResourceId& res);
	/// sets the sun color
	void SetSunColor(const Math::float4& color);
	/// sets if the sun should be global light position relative
	void SetSunLightRelative(bool b);
	/// sets the sun position, assumes the above function has been called with true, otherwise has no effect
	void SetSunPosition(const Math::float4& pos);

}; 
} // namespace Godrays
//------------------------------------------------------------------------------