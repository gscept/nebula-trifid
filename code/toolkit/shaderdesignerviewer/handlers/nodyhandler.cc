//------------------------------------------------------------------------------
//  nodyhandler.cc
//  (C) 2011 gscept
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "handlers/nodyhandler.h"
#include "graphics/graphicsinterface.h"
#include "graphics/graphicsprotocol.h"
#include "shaderdesignerviewer/shaderdesignerviewerapplication.h"
#include "apprender/viewerapplication.h"
#include "graphics/modelentity.h"

namespace Tools
{
__ImplementClass(Tools::NodyHandler, 'NDHL', Remote::RemoteRequestHandler);

using namespace Remote;
using namespace Util;
using namespace Graphics;

//------------------------------------------------------------------------------
/**
*/
NodyHandler::NodyHandler()
{
	this->SetControllerName("Nody");
}

//------------------------------------------------------------------------------
/**
*/
NodyHandler::~NodyHandler()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
void 
NodyHandler::HandleRequest( const Ptr<RemoteRequest>& request )
{
	const Ptr<IO::XmlReader>& requestReader = request->GetXmlReader();
	requestReader->Open();

	if (requestReader->SetToFirstChild()) do 
	{
		String message = requestReader->GetCurrentNodeName();
		if (message == "UpdateShader")
		{
			String shader = requestReader->GetString("name");
			Ptr<InvalidateShader> invShaderMsg = InvalidateShader::Create();
			invShaderMsg->SetShaderName("shd:" + shader + ".sdh");
			GraphicsInterface::Instance()->Send(invShaderMsg.cast<Messaging::Message>());
		}
		else if (message == "UpdateMaterial")
		{
			Ptr<InvalidateMaterial> invMaterialMsg = InvalidateMaterial::Create();
			invMaterialMsg->SetMaterialName(requestReader->GetString("name"));
			GraphicsInterface::Instance()->Send(invMaterialMsg);
		}
		else if (message == "SetMaterial")
		{
			String material = requestReader->GetString("name");
			String node = requestReader->GetString("node");
			
			Ptr<UpdModelNodeMaterial> setMatMsg = UpdModelNodeMaterial::Create();			
			setMatMsg->SetMaterial(material);
			setMatMsg->SetModelNodeName(node);

			this->modelEntity->SendMsg(setMatMsg.downcast<Graphics::GraphicsEntityMessage>());
		}
		else if (message == "SetTexture")
		{
			String resource = requestReader->GetString("resource");
			String node = requestReader->GetString("node");
			String destinations = requestReader->GetString("destinations");

			Array<String> destinationsCompact = destinations.Tokenize(";");
			for (int i = 0; i < destinationsCompact.Size(); i++)
			{
				String destination = destinationsCompact[i];
				Ptr<UpdMaterialModelNodeInstanceTexture> setTexMsg = UpdMaterialModelNodeInstanceTexture::Create();
				setTexMsg->SetModelNodeInstanceName(node);
				setTexMsg->SetTextureName(destination);
				setTexMsg->SetTextureResource(resource);
				this->modelEntity->SendMsg(setTexMsg.downcast<Graphics::GraphicsEntityMessage>());
			}

		}
		else if (message == "SetVariable")
		{
			String var = requestReader->GetString("name");
			String value = requestReader->GetString("value");
			String node = requestReader->GetString("node");
			Ptr<UpdMaterialModelNodeInstanceShaderVariable> setVarMsg = UpdMaterialModelNodeInstanceShaderVariable::Create();

			Util::Variant variant;
			setVarMsg->SetModelNodeInstanceName(node);
			setVarMsg->SetSemantic(var);
			setVarMsg->SetValue(variant);
			this->modelEntity->SendMsg(setVarMsg.downcast<Graphics::GraphicsEntityMessage>());			
		}
		else if (message == "SetModel")
		{
			String model = requestReader->GetString("name");
			Ptr<Graphics::ModelEntity> modelEntity = ((ShaderDesignerViewerApplication*)App::ViewerApplication::Instance())->GetModel();
			if (modelEntity->IsValid())
			{
				modelEntity->SetResourceId("mdl:" + model);
			}
		}
	} 
	while(requestReader->SetToNextChild());

	requestReader->Close();
	request->SetStatus(RemoteStatus::OK);
}

} // namespace Tools