#pragma once
//------------------------------------------------------------------------------
/**
    @class Base::EnvironmentPluginBase
    
    Base class for environment effect plugins.
    
    (C) 2013 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------
#include "core/refcounted.h"
#include "graphics/cameraentity.h"
namespace Base
{
class EnvironmentModuleBase : public Core::RefCounted
{
	__DeclareClass(EnvironmentModuleBase);
public:
	/// constructor
	EnvironmentModuleBase();
	/// destructor
	virtual ~EnvironmentModuleBase();

	/// setup module
	virtual void Setup();
	/// discard module
	virtual void Discard();

	/// call before rendering
	virtual void BeginFrame(const Ptr<Graphics::CameraEntity>& cam);
	/// render
	virtual void Render();
	/// call after rendering
	virtual void EndFrame();	
	/// set attached/detached on module
	virtual void SetAttached(bool);

	/// handles messages
	virtual void HandleMessage(const Ptr<Messaging::Message>& msg);

	/// returns true if module is enabled
	bool IsEnabled() const;
	
protected:
	/// enables/disables module
	virtual void SetEnabled(bool b);

	bool isEnabled;
}; 


//------------------------------------------------------------------------------
/**
*/
inline void 
EnvironmentModuleBase::SetEnabled( bool b )
{
	this->isEnabled = b;
}

//------------------------------------------------------------------------------
/**
*/
inline bool 
EnvironmentModuleBase::IsEnabled() const
{
	return this->isEnabled;
}

} // namespace Base
//------------------------------------------------------------------------------