#pragma once
//------------------------------------------------------------------------------
/**
	@class Grid::GridAddon
	
	Renders an infinite grid in X-Z space.
	
	(C) 2012-2016 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------
#include "core/refcounted.h"
#include "core/singleton.h"
#include "gridrtplugin.h"
namespace Grid
{
class GridAddon : public Core::RefCounted
{
	__DeclareClass(GridAddon);
	__DeclareSingleton(GridAddon);
public:
	/// constructor
	GridAddon();
	/// destructor
	virtual ~GridAddon();

	/// setup
	void Setup();
	/// discard
	void Discard();

	/// set if grid should be visible
	void SetVisible(bool b);
	/// set grid size
	void SetGridSize(float size);	

private:
	Ptr<GridRTPlugin> plugin;
};
} // namespace Grid