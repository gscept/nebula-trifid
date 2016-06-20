#pragma once
//------------------------------------------------------------------------------
/**
	@class Grid::GridAddon
	
	Renders a huge X-Z plane for clouds.
	
	(C) 2012-2016 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------
#include "core/refcounted.h"
#include "core/singleton.h"
#include "cloudsrtplugin.h"
namespace Clouds
{
class CloudsAddon : public Core::RefCounted
{
	__DeclareClass(CloudsAddon);
	__DeclareSingleton(CloudsAddon);
public:
	/// constructor
	CloudsAddon();
	/// destructor
	virtual ~CloudsAddon();

	/// setup
	void Setup();
	/// discard
	void Discard();

	/// set if grid should be visible
	void SetVisible(bool b);
	/// set cloud settings, index dictates which cloud layer to modify (valid values: 0-2)
	void SetCloudSettings(const CloudsRTPlugin::CloudSettings& settings, IndexT index);

private:
	Ptr<CloudsRTPlugin> plugin;
};
} // namespace Grid