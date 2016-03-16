//------------------------------------------------------------------------------
//  basehandler.cc
//  (C) 2012-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "basehandler.h"

namespace Widgets
{
__ImplementClass(Widgets::BaseHandler, 'BAHN', Core::RefCounted);

//------------------------------------------------------------------------------
/**
*/
BaseHandler::BaseHandler() :
	isSetup(false)
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
BaseHandler::~BaseHandler()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
void 
BaseHandler::Cleanup()
{
    // empty, override in subclass
}

//------------------------------------------------------------------------------
/**
*/
void 
BaseHandler::Setup()
{
	n_assert(!this->isSetup);
	this->isSetup = true;
}

//------------------------------------------------------------------------------
/**
*/
bool 
BaseHandler::Discard()
{
	n_assert(this->isSetup);
	this->isSetup = false;
	return true;
}

//------------------------------------------------------------------------------
/**
*/
void
BaseHandler::OnFrame()
{
	// override in subclass
}

} // namespace Widgets