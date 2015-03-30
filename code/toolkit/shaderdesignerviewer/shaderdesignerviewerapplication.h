#pragma once
#ifndef TOOLS_SHADERDESIGNERVIEWER_H
#define TOOLS_SHADERDESIGNERVIEWER_H
#include "apprender/viewerapplication.h"
#include "graphics/modelentity.h"
#include "graphics/globallightentity.h"
#include "remote/remotecontrolproxy.h"
#include "remote/remoteinterface.h"

//------------------------------------------------------------------------------
/**
    @class Tools::ShaderDesignerViewer
    
    Nebula3 test viewer app.

    (C) 2011 gsCEPT
*/
namespace  Tools
{
	class ShaderDesignerViewerApplication : public App::ViewerApplication
	{
	public:
		/// constructor
		ShaderDesignerViewerApplication(void);
		/// destructor
		virtual ~ShaderDesignerViewerApplication(void);
		/// open application
		virtual bool Open();
		/// close application
		virtual void Close();

		/// returns the model
		const Ptr<Graphics::ModelEntity>& GetModel() const;

	private:
		/// each frame actions		
		virtual void OnUpdateFrame();
		/// processes on-input actions
		virtual void OnProcessInput();

		Ptr<Graphics::ModelEntity> model;
		Ptr<Graphics::GlobalLightEntity> globalLight;

		/// remote access variables
		Ptr<Remote::RemoteControlProxy> remoteControlProxy;
		Ptr<Remote::RemoteInterface> remoteInterface;

	};

}

#endif // TOOLS_SHADERDESIGNERVIEWER_H