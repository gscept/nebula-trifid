#pragma once
//------------------------------------------------------------------------------
/**
	@class Silhouette::SilhouetteAddon
	
	Renders an silhouette around selected modelentities
	
	(C) 2012-2015 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------
#include "core/refcounted.h"
#include "core/singleton.h"
#include "silhouettertplugin.h"
#include "graphics/modelentity.h"
namespace Silhouette
{
class SilhouetteAddon : public Core::RefCounted
{
	__DeclareClass(SilhouetteAddon);
	__DeclareSingleton(SilhouetteAddon);
public:
	/// constructor
	SilhouetteAddon();
	/// destructor
	virtual ~SilhouetteAddon();

	/// setup
	void Setup();
	/// discard
	void Discard();

	/// set if silhouette should be visible
	void SetVisible(bool b);
	/// set models for a specific group to be rendered with colour. if the group doesnt exist it will be added
	void SetModels(const Util::String& group, const Util::Array<Ptr<Graphics::ModelEntity>>& mdls, const Math::float4& colour);
	/// clear a group and remove it from rendering
	void ClearModelGroup(const Util::String& group);

private:
	Ptr<SilhouetteRTPlugin> plugin;
};
} // namespace Silhouette