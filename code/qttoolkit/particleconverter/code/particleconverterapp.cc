//------------------------------------------------------------------------------
// surfacegeneratorapp.cc
// (C) 2015-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "particleconverterapp.h"


namespace ToolkitUtil
{


//------------------------------------------------------------------------------
/**
*/
ParticleConverterApp::ParticleConverterApp()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
ParticleConverterApp::~ParticleConverterApp()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
bool
ParticleConverterApp::Open()
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
ParticleConverterApp::Exit()
{
	// hmm, do nothing since app will close with window?
}

} // namespace ToolkitUtil