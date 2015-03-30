#pragma once
//------------------------------------------------------------------------------
/**
    @class EffectsFeature::GraphicsEffect
    
    Creates a graphics entity as an effect.
    
    (C) 2013 Gustav Sterbrant
*/
//------------------------------------------------------------------------------
#include "effect.h"
#include "graphics/modelentity.h"
namespace EffectsFeature
{
class GraphicsEffect : public Effect
{
	__DeclareClass(GraphicsEffect);
public:
	/// constructor
	GraphicsEffect();
	/// destructor
	virtual ~GraphicsEffect();

	/// called when effect start
	void OnActivate(Timing::Time time);
	/// called when effect ends
	void OnDeactivate();

	/// sets the resource
	void SetResource(const Resources::ResourceId& res);
	/// gets the resource
	const Resources::ResourceId& GetResource() const;

private:
	Ptr<Graphics::ModelEntity> entity;
	Resources::ResourceId resource;
}; 

//------------------------------------------------------------------------------
/**
*/
inline void 
EffectsFeature::GraphicsEffect::SetResource( const Resources::ResourceId& res )
{
	this->resource = res;
}

//------------------------------------------------------------------------------
/**
*/
inline const Resources::ResourceId& 
EffectsFeature::GraphicsEffect::GetResource() const
{
	return this->resource;
}
} // namespace FX
//------------------------------------------------------------------------------