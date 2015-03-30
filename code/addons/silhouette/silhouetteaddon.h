#pragma once
//------------------------------------------------------------------------------
/**
	@class Grid::GridAddon
	
	Renders an infinite grid in X-Z space.
	
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

	/// set if grid should be visible
	void SetVisible(bool b);
	/// set model to be rendered with a silhouette (only one is allowed)
	void SetModels(const Util::Array<Ptr<Graphics::ModelEntity>>& mdls);
	/// set color to use for silhouette
	void SetColor(const Math::float4& col);

private:
	Ptr<SilhouetteRTPlugin> plugin;
};
} // namespace Silhouette