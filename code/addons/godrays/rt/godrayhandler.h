#pragma once
//------------------------------------------------------------------------------
/**
    @class Godrays::GodrayHandler
    
    Handles messages sent to the godray render module
    
    (C) 2013 Gustav Sterbrant
*/
//------------------------------------------------------------------------------
#include "interface/interfacehandlerbase.h"
#include "godrayprotocol.h"
namespace Godrays
{
class GodrayHandler : public Interface::InterfaceHandlerBase
{
	__DeclareClass(GodrayHandler);
public:
	/// constructor
	GodrayHandler();
	/// destructor
	virtual ~GodrayHandler();

	/// handles message
	virtual bool HandleMessage(const Ptr<Messaging::Message>& msg);
	
private:
	/// handle sun texture message
	void OnSetSunTexture(const Ptr<SetSunTexture>& msg);
	/// handle sun color message
	void OnSetSunColor(const Ptr<SetSunColor>& msg);
	/// handle sun position relativity
	void OnSetSunGlobalLightRelative(const Ptr<SetSunGlobalLightRelative>& msg);
	/// handle sun position
	void OnSetSunPosition(const Ptr<SetSunPosition>& msg);

}; 
} // namespace Godrays
//------------------------------------------------------------------------------