//------------------------------------------------------------------------------
//  environmentserver.cc
//  (C) 2013 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "environmentserver.h"
#include "base/environmentmodulebase.h"
#include "modules/aomodule.h"
#include "graphics/graphicsprotocol.h"

using namespace Base;
using namespace Graphics;
namespace Environment
{
__ImplementClass(Environment::EnvironmentServer, 'ENVS', Core::RefCounted);
__ImplementSingleton(Environment::EnvironmentServer);


//------------------------------------------------------------------------------
/**
*/
EnvironmentServer::EnvironmentServer() :
	isOpen(false)
{
	__ConstructSingleton;
}

//------------------------------------------------------------------------------
/**
*/
EnvironmentServer::~EnvironmentServer()
{
	if (this->IsOpen())
	{
		this->Close();
	}
	__DestructSingleton;
}

//------------------------------------------------------------------------------
/**
*/
bool
EnvironmentServer::Open()
{
	n_assert(!this->IsOpen());

	// modules should be created from somewhere else, only problem is that the main frame shader needs the textures in the modules to work properly
	Ptr<AOModule> aoModule = AOModule::Create();
	this->RegisterModule(aoModule.upcast<Base::EnvironmentModuleBase>());

	this->isOpen = true;
	return true;
}

//------------------------------------------------------------------------------
/**
*/
void
EnvironmentServer::Close()
{
	n_assert(this->IsOpen());

	// shut down modules
	IndexT i;
	for (i = 0; i < this->modules.Size(); i++)
	{
		this->modules.ValueAtIndex(i)->Discard();
	}
	this->modules.Clear();
	this->isOpen = false;
}

//------------------------------------------------------------------------------
/**
*/
void 
EnvironmentServer::RegisterModule( const Ptr<EnvironmentModuleBase>& module )
{
	n_assert(!this->modules.Contains(module->GetRtti()));
	this->modules.Add(module->GetRtti(), module);
	module->Setup();
}

//------------------------------------------------------------------------------
/**
*/
void 
EnvironmentServer::BeginFrame( const Ptr<CameraEntity>& cam )
{
	n_assert(this->IsOpen());

	// call modules
	IndexT i;
	for (i = 0; i < this->modules.Size(); i++)
	{
		Ptr<EnvironmentModuleBase> module = this->modules.ValueAtIndex(i);
		if (module->IsEnabled())
		{
			module->BeginFrame(cam);
		}
		
	}
}

//------------------------------------------------------------------------------
/**
*/
void 
EnvironmentServer::Render()
{
	n_assert(this->IsOpen());

	// call modules
	IndexT i;
	for (i = 0; i < this->modules.Size(); i++)
	{
		Ptr<EnvironmentModuleBase> module = this->modules.ValueAtIndex(i);
		if (module->IsEnabled())
		{
			module->Render();
		}
	}
}

//------------------------------------------------------------------------------
/**
*/
void 
EnvironmentServer::EndFrame()
{
	n_assert(this->IsOpen());

	// call modules
	IndexT i;
	for (i = 0; i < this->modules.Size(); i++)
	{
		Ptr<EnvironmentModuleBase> module = this->modules.ValueAtIndex(i);
		if (module->IsEnabled())
		{
			module->EndFrame();
		}
	}
}

//------------------------------------------------------------------------------
/**
*/
void 
EnvironmentServer::HandleMessage( const Ptr<Messaging::Message>& msg )
{
	n_assert(this->IsOpen());

	if(msg->CheckId(Graphics::SetEnvironmentAttached::Id))
	{
		// call modules
		IndexT i;
		for (i = 0; i < this->modules.Size(); i++)
		{
			this->modules.ValueAtIndex(i)->SetAttached((msg.cast<Graphics::SetEnvironmentAttached>())->GetAttached());
		}
		msg->SetHandled(true);
	}
	else
	{
		// call modules
		IndexT i;
		for (i = 0; i < this->modules.Size(); i++)
		{
			this->modules.ValueAtIndex(i)->HandleMessage(msg);
		}
	}	

	// always assume message gets handled
	n_assert(msg->Handled());
}
}
