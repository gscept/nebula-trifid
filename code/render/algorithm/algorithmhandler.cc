//------------------------------------------------------------------------------
//  algorithmhandler.cc
//  (C) 2013 gscept
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "algorithmhandler.h"
#include "algorithmbase.h"
#include "algorithm/algorithmprotocol.h"

namespace Algorithm
{
__ImplementClass(Algorithm::AlgorithmHandler, 'ALHA', Interface::InterfaceHandlerBase);

//------------------------------------------------------------------------------
/**
*/
AlgorithmHandler::AlgorithmHandler() :
	algorithm(0)
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
AlgorithmHandler::~AlgorithmHandler()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
bool 
AlgorithmHandler::HandleMessage( const Ptr<Messaging::Message>& msg )
{
	return this->algorithm->HandleMessage(msg);
}


} // namespace Algorithm