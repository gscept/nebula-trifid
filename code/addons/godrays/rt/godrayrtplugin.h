#pragma once
//------------------------------------------------------------------------------
/**
    @class Godrays::GodrayRTPlugin
    
    Server-side plugin for Godray rendering.
    
    (C) 2013 Gustav Sterbrant
*/
//------------------------------------------------------------------------------
#include "rendermodules/rt/rtplugin.h"
#include "godrayhandler.h"
#include "godrayserver.h"
namespace Godrays
{
class GodrayRTPlugin : public RenderModules::RTPlugin
{
	__DeclareClass(GodrayRTPlugin);
public:
	/// constructor
	GodrayRTPlugin();
	/// destructor
	virtual ~GodrayRTPlugin();

	/// register module
	void OnRegister();
	/// unregister module
	void OnUnregister();
	/// handle rendering
	void OnRenderBefore(IndexT frameId, Timing::Time time);
	/// handle updating after rendering
	void OnRenderAfter(IndexT frameId, Timing::Time time);

private:
	Ptr<GodrayHandler> handler;
	Ptr<GodrayServer> server;

}; 
} // namespace Godrays
//------------------------------------------------------------------------------