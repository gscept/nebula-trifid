//------------------------------------------------------------------------------
//  shaderdesignerviewerapplication.cc
//  (C) 2011 gsCEPT
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "shaderdesignerviewerapplication.h"
#include "handlers/nodyhandler.h"
#include "remote/remoteinterface.h"
#include "remote/remotecontrolproxy.h"
#include "graphics/graphicsprotocol.h"
#include "messaging/message.h"
#include "input/keyboard.h"
#include "input/gamepad.h"


namespace Tools
{
using namespace Remote;
using namespace Graphics;
using namespace Math;
using namespace Input;

	//------------------------------------------------------------------------------
	/**
	*/
	ShaderDesignerViewerApplication::ShaderDesignerViewerApplication(void)
	{
		// empty
	}

	//------------------------------------------------------------------------------
	/**
	*/
	ShaderDesignerViewerApplication::~ShaderDesignerViewerApplication(void)
	{
		if (this->IsOpen())
		{
			this->Close();
		}
	}

		//------------------------------------------------------------------------------
	/**
	*/
	bool 
	ShaderDesignerViewerApplication::Open()
	{
		n_assert(!this->IsOpen());
		if (ViewerApplication::Open())
		{

			matrix44 lightTransform = matrix44::rotationx(n_deg2rad(-45.0f));
			this->globalLight = GlobalLightEntity::Create();
			this->globalLight->SetTransform(lightTransform);
			this->globalLight->SetColor(float4(2.9f, 2.9f, 2.9f, 1.0f));
			this->globalLight->SetBackLightColor(float4(0.6f, 0.6f, 0.6f, 0.0f));
			this->globalLight->SetCastShadows(false);
			this->stage->AttachEntity(this->globalLight.cast<GraphicsEntity>());

			this->model = ModelEntity::Create();
			this->model->SetTransform(matrix44::translation(0.0, 0.0, 0.0f));
			this->model->SetResourceId("mdl:system/shadingsphere.n3");
			this->stage->AttachEntity(this->model.cast<GraphicsEntity>());

			this->remoteInterface = RemoteInterface::Create();
			this->remoteInterface->SetPortNumber(13032);
			this->remoteInterface->Open();
			this->remoteControlProxy = RemoteControlProxy::Create();
			this->remoteControlProxy->Open();
			Ptr<NodyHandler> handler = NodyHandler::Create();
			handler->SetModelEntity(this->model);
			this->remoteControlProxy->AttachRequestHandler(handler.downcast<Remote::RemoteRequestHandler>());
			
			return true;
		}
		return false;
	}

		//------------------------------------------------------------------------------
	/**
	*/
	void 
	ShaderDesignerViewerApplication::Close()
	{
		this->stage->RemoveEntity(this->globalLight.cast<GraphicsEntity>());
		this->stage->RemoveEntity(this->model.cast<GraphicsEntity>());
		this->globalLight = 0;
		this->model = 0;
		this->remoteControlProxy->Close();
		this->remoteControlProxy = 0;
		this->remoteInterface->Close();
		this->remoteInterface = 0;
		ViewerApplication::Close();
	}

		//------------------------------------------------------------------------------
	/**
	*/
	void 
	ShaderDesignerViewerApplication::OnUpdateFrame()
	{
		this->remoteControlProxy->HandlePendingRequests();
		ViewerApplication::OnUpdateFrame();
	}

	//------------------------------------------------------------------------------
	/**
	*/
	void 
	ShaderDesignerViewerApplication::OnProcessInput()
	{
		const Ptr<Keyboard>& kbd = InputServer::Instance()->GetDefaultKeyboard();
		const Ptr<GamePad>& gamePad = InputServer::Instance()->GetDefaultGamePad(0);

		ViewerApplication::OnProcessInput();
	}

	//------------------------------------------------------------------------------
	/**
	*/
	const Ptr<Graphics::ModelEntity>& 
	ShaderDesignerViewerApplication::GetModel() const
	{
		return this->model;
	}
}
