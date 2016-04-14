#pragma once
//------------------------------------------------------------------------------
/**
	App entry point for surface generator app.
	
	(C) 2015-2016 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------
#include "core/refcounted.h"
#include "app/application.h"
#include "particleconverterwindow.h"

namespace ToolkitUtil
{
class ParticleConverterApp : public App::Application
{
public:
	/// constructor
	ParticleConverterApp();
	/// destructor
	~ParticleConverterApp();

	/// opens the app
	virtual bool Open();
	/// close the app
	virtual void Exit();
private:
	ParticleConverterWindow window;
};
} // namespace ToolkitUtil