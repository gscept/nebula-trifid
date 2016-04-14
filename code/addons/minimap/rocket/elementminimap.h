#pragma once

//------------------------------------------------------------------------------
/**
	@class Rocket::MiniMap

	(C) 2016 Individual contributors, see AUTHORS file
*/
#include <Rocket/Core/Element.h>
#include <Rocket/Core/Geometry.h>
#include <Rocket/Core/Texture.h>
#include "Rocket/Core/EventListenerInstancer.h"
#include "Rocket/Core/EventListener.h"


namespace Rocket
{
namespace MiniMap 
{


class ElementMiniMap : public Core::Element
{
public:
	/// Constructs a new ElementProgressBar. This should not be called directly; use the Factory instead.
	/// @param[in] tag The tag the element was declared as in RML.
	ElementMiniMap(const Rocket::Core::String& tag);
	///
	virtual ~ElementMiniMap();
		
protected:
	/// Called during render after backgrounds, borders, decorators, but before children, are rendered.
	virtual void OnRender();
	/// 
	bool GetIntrinsicDimensions(Core::Vector2f& dimensions);
	///
	virtual void OnPropertyChange(const Rocket::Core::PropertyNameList& changed_properties);

private:
	/// Called when value has changed.
	void GenerateGeometry();
	
	Core::Geometry geometry;
	Core::Geometry overlayGeometry;
	Core::Texture *texture;
	Core::Texture *overlay;
	Core::Vector2f dimensions;
	bool geometry_dirty;

};

}
}
