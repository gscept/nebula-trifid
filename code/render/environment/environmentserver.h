#pragma once
//------------------------------------------------------------------------------
/**
    @class Environment::EnvironmentServer
    
    Handles  render effects such as AO and Picking.
    
    (C) 2013 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------
#include "core/singleton.h"
#include "core/refcounted.h"
#include "graphics/cameraentity.h"

namespace Base
{
	class EnvironmentModuleBase;
}
namespace Environment
{
class EnvironmentServer : public Core::RefCounted
{
	__DeclareClass(EnvironmentServer)
	__DeclareSingleton(EnvironmentServer);	
public:
	/// constructor
	EnvironmentServer();
	/// destructor
	virtual ~EnvironmentServer();
	/// open
	bool Open();
	/// close
	void Close();
	/// returns true if server is open
	bool IsOpen() const;

	/// register an environment plugin module
	void RegisterModule(const Ptr<Base::EnvironmentModuleBase>& module);

	/// begin frame
	void BeginFrame(const Ptr<Graphics::CameraEntity>& cam);
	/// render
	void Render();
	/// end frame
	void EndFrame();

	/// handles incoming messages by propagating them to the modules
	void HandleMessage(const Ptr<Messaging::Message>& msg);

private:
	Util::Dictionary<Core::Rtti*, Ptr<Base::EnvironmentModuleBase> > modules;
	bool isOpen;
}; 

//------------------------------------------------------------------------------
/**
*/
inline bool
EnvironmentServer::IsOpen() const
{
	return this->isOpen;
}

} // namespace Environment
//------------------------------------------------------------------------------