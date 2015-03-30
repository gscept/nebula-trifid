#pragma once
//------------------------------------------------------------------------------
/**
    @class Tools::NodyHandler
    
    Remote request handler used to handle messages from Nody
    
    (C) 2012 gscept
*/
#include "remote\remoterequesthandler.h"
#include "graphics/modelentity.h"

//------------------------------------------------------------------------------
namespace Tools
{
class NodyHandler : public Remote::RemoteRequestHandler
{
	__DeclareClass(NodyHandler);
public:
	/// constructor
	NodyHandler();
	/// destructor
	virtual ~NodyHandler();

	/// sets the model entity
	void SetModelEntity(const Ptr<Graphics::ModelEntity>& modelEntity);

private:
	/// request handling for Nody requests
	void HandleRequest(const Ptr<Remote::RemoteRequest>& request);
	
	Ptr<Graphics::ModelEntity> modelEntity;
}; 


//------------------------------------------------------------------------------
/**
*/
inline void 
NodyHandler::SetModelEntity( const Ptr<Graphics::ModelEntity>& modelEntity )
{
	this->modelEntity = modelEntity;
}

} // namespace Tools
//------------------------------------------------------------------------------