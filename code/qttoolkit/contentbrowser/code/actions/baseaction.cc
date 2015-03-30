//------------------------------------------------------------------------------
//  baseaction.cc
//  (C) 2012-2015 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "baseaction.h"

namespace Actions
{
__ImplementClass(Actions::BaseAction, 'BSAC', Core::RefCounted);

//------------------------------------------------------------------------------
/**
*/
BaseAction::BaseAction() :
	currentVersion(0),
	finalVersion(0)
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
BaseAction::~BaseAction()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
void 
BaseAction::Cleanup()
{
    n_error("BaseAction::Cleanup() called!\n");
}

//------------------------------------------------------------------------------
/**
	Override in subclass
*/
void 
BaseAction::Undo()
{
	n_error("BaseAction::Undo() called!\n");
}

//------------------------------------------------------------------------------
/**
	Override in subclass
*/
void 
BaseAction::Redo()
{
	n_error("BaseAction::Redo() called!\n");
}

//------------------------------------------------------------------------------
/**
	Override in subclass
*/
void 
BaseAction::Do()
{
	n_error("BaseAction::Do() called!\n");
}

//------------------------------------------------------------------------------
/**
	Override in subclass
*/
void 
BaseAction::Discard()
{
	this->finalVersion = 0;
	this->currentVersion = 0;
}

//------------------------------------------------------------------------------
/**
*/
bool 
BaseAction::IsFinal() const
{
	return this->finalVersion == this->currentVersion;
}

//------------------------------------------------------------------------------
/**
*/
void 
BaseAction::TagAsFinal()
{
	this->finalVersion = this->currentVersion;
}
} // namespace Actions