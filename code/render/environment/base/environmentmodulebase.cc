//------------------------------------------------------------------------------
//  environmentpluginbase.cc
//  (C) 2013 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "environmentmodulebase.h"
#include "graphics/cameraentity.h"

namespace Base
{
__ImplementClass(Base::EnvironmentModuleBase, 'ENPB', Core::RefCounted);

//------------------------------------------------------------------------------
/**
*/
EnvironmentModuleBase::EnvironmentModuleBase() :
	isEnabled(true)
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
EnvironmentModuleBase::~EnvironmentModuleBase()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
void 
EnvironmentModuleBase::Setup()
{
	// implement in subclass
}

//------------------------------------------------------------------------------
/**
*/
void 
EnvironmentModuleBase::Discard()
{
	// implement in subclass
}

//------------------------------------------------------------------------------
/**
*/
void 
EnvironmentModuleBase::BeginFrame( const Ptr<Graphics::CameraEntity>& cam )
{
	// override in subclass
}

//------------------------------------------------------------------------------
/**
*/
void 
EnvironmentModuleBase::Render()
{
	// override in subclass
}

//------------------------------------------------------------------------------
/**
*/
void 
EnvironmentModuleBase::EndFrame()
{
	// override in subclass
}

//------------------------------------------------------------------------------
/**
*/
void 
EnvironmentModuleBase::SetAttached(bool)
{
	// override in subclass
}


//------------------------------------------------------------------------------
/**
*/
void 
EnvironmentModuleBase::HandleMessage( const Ptr<Messaging::Message>& msg )
{
	// implement in subclass
}

} // namespace Base