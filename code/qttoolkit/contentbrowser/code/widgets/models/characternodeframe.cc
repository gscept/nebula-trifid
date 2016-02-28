//------------------------------------------------------------------------------
//  characternodeframe.cc
//  (C) 2012-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "characternodeframe.h"

namespace Widgets
{

//------------------------------------------------------------------------------
/**
*/
CharacterNodeFrame::CharacterNodeFrame()
{
	/// create handlers
	this->itemHandler = CharacterNodeHandler::Create();

	/// setup ui
	this->ui.setupUi(this);

	/// set item handler stuff
	this->itemHandler->SetUI(&this->ui);
}

//------------------------------------------------------------------------------
/**
*/
CharacterNodeFrame::~CharacterNodeFrame()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
void 
CharacterNodeFrame::Discard()
{
	this->itemHandler->Discard();
	this->itemHandler = 0;
}
} // namespace Widgets