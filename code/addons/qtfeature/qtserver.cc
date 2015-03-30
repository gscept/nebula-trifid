//------------------------------------------------------------------------------
//  qtserver.cc
//  (C) 2013-2015 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "addons/qtfeature/qtserver.h"
#include "app/application.h"
#include <QEventLoop>

namespace QtFeature
{
__ImplementClass(QtFeature::QtServer, 'QTSV', Core::RefCounted);
__ImplementSingleton(QtFeature::QtServer);

using namespace Util;

//------------------------------------------------------------------------------
/**
*/
QtServer::QtServer():application(NULL)    
{
    __ConstructSingleton;    
}

//------------------------------------------------------------------------------
/**
*/
QtServer::~QtServer()
{
	n_assert(this->application == NULL);
    __DestructSingleton;
}

//------------------------------------------------------------------------------
/**
*/
void
QtServer::Open()
{
	n_assert(this->application == NULL);

    // use software rasterizer
    QApplication::setGraphicsSystem("raster");

    const Util::CommandLineArgs& args = App::Application::Instance()->GetCmdLineArgs();
	if (args.IsEmpty())
	{
		this->argc = 0;
		this->argv = 0;
	}
	else
	{
		this->name = args.GetCmdName().AsCharPtr();
		this->argv = &(this->name);
		this->argc = 1;
	}    
    this->application = new QApplication(argc, (char**)argv);
}

//------------------------------------------------------------------------------
/**
*/
void
QtServer::Close()
{
    n_assert(this->application != NULL);
	this->application->exit(0);
	delete this->application;
    this->application = NULL;
}

//------------------------------------------------------------------------------
/**
*/
void
QtServer::Trigger()
{
    n_assert(this->application != NULL);

    // run qt event loop
    this->application->processEvents();

}



} // namespace QtFeature