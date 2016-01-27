#pragma once
//------------------------------------------------------------------------------
/**
	App entry point for surface generator app.
	
	(C) 2015-2016 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------
#include "core/refcounted.h"
#include "app/application.h"
#include "surfacegeneratorwindow.h"

namespace ToolkitUtil
{
class SurfaceGeneratorApp : public App::Application
{
public:
	/// constructor
	SurfaceGeneratorApp();
	/// destructor
	~SurfaceGeneratorApp();

	/// opens the app
	virtual bool Open();
	/// close the app
	virtual void Exit();
private:
	SurfaceGeneratorWindow window;
};
} // namespace ToolkitUtil