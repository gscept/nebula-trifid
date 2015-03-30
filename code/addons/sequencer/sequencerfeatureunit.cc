//------------------------------------------------------------------------------
//  sequenzerfeatureunit.cc
//  (C) 2007 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "sequencerfeatureunit.h"

#include "sequencermanager.h"

namespace Sequencer
{
__ImplementClass(Sequencer::SequencerFeatureUnit, 'SFUN', FeatureUnit);
__ImplementSingleton(Sequencer::SequencerFeatureUnit);

//------------------------------------------------------------------------------
/**
*/
SequencerFeatureUnit::SequencerFeatureUnit():
		sequencManager(0)
{
    
}

//------------------------------------------------------------------------------
/**
*/
SequencerFeatureUnit::~SequencerFeatureUnit()
{

}

//------------------------------------------------------------------------------
/**
*/
void
SequencerFeatureUnit::OnActivate()
{
	this->sequencManager = SequencerManager::Create();
	this->AttachManager(this->sequencManager);
}

//------------------------------------------------------------------------------
/**
*/
void
SequencerFeatureUnit::OnDeactivate()
{
	this->RemoveManager(this->sequencManager);
	this->sequencManager = 0;
}

//------------------------------------------------------------------------------
/**
*/
void
SequencerFeatureUnit::OnStart()
{

}

//------------------------------------------------------------------------------
/**
*/
void
SequencerFeatureUnit::OnBeginFrame()
{

}

//------------------------------------------------------------------------------
/**
*/
void
SequencerFeatureUnit::OnFrame()
{

}

//------------------------------------------------------------------------------
/**
*/
void
SequencerFeatureUnit::OnEndFrame()
{

}

}//End namespace Sequencer