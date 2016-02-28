//------------------------------------------------------------------------------
//  baseaction.cc
//  (C) 2012-2016 Individual contributors, see AUTHORS file
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
	previousVersion(0),
	currentVersion(0),
	numVersions(0),
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
	this->previousVersion = this->currentVersion;
	this->currentVersion = Math::n_max(this->currentVersion - 1, 0);
	this->Do();
}

//------------------------------------------------------------------------------
/**
	Override in subclass
*/
void 
BaseAction::Redo()
{
	this->previousVersion = this->currentVersion;
	this->currentVersion = Math::n_min(this->currentVersion + 1, numVersions - 1);
	this->Do();
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
*/
void
BaseAction::DoAndMakeCurrent()
{
	this->previousVersion = this->currentVersion;
	this->currentVersion = Math::n_min(this->currentVersion + 1, numVersions - 1);
	this->Do();
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
	this->numVersions = 0;
	this->previousVersion = 0;
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