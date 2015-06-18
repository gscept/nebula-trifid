//------------------------------------------------------------------------------
//  particlenodeframe.cc
//  (C) 2012 Gustav Sterbrant
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "particlenodeframe.h"

namespace Widgets
{

//------------------------------------------------------------------------------
/**
*/
ParticleNodeFrame::ParticleNodeFrame()
{
	// create handlers
	this->itemHandler = ParticleNodeHandler::Create();

	// setup ui
	this->ui.setupUi(this);

	// set item handler stuff
	this->itemHandler->SetUi(&this->ui);
}

//------------------------------------------------------------------------------
/**
*/
ParticleNodeFrame::~ParticleNodeFrame()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
void 
ParticleNodeFrame::Discard()
{
	this->itemHandler->Discard();
	this->itemHandler = 0;
}

//------------------------------------------------------------------------------
/**
*/
void
ParticleNodeFrame::Refresh()
{
	this->itemHandler->Refresh();
}

} // namespace Particles