//------------------------------------------------------------------------------
// surfacegeneratorapp.cc
// (C) 2015-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "surfacegeneratorapp.h"


namespace ToolkitUtil
{


//------------------------------------------------------------------------------
/**
*/
SurfaceGeneratorApp::SurfaceGeneratorApp()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
SurfaceGeneratorApp::~SurfaceGeneratorApp()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
bool
SurfaceGeneratorApp::Open()
{
	n_assert(!this->IsOpen());
	if (Application::Open())
	{
		this->window.show();
		return true;
	}
	return false;
}

//------------------------------------------------------------------------------
/**
*/
void
SurfaceGeneratorApp::Exit()
{
	// hmm, do nothing since app will close with window?
}

} // namespace ToolkitUtil