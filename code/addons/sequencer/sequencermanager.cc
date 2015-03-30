//------------------------------------------------------------------------------
//  sequenzermanager.cc
//  (C) 2007 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "sequencermanager.h"

#include "tracksequencer.h"

namespace Sequencer
{
__ImplementClass(Sequencer::SequencerManager, 'SMGR',Game::Manager);
__ImplementSingleton(Sequencer::SequencerManager);

//------------------------------------------------------------------------------
/**
*/
SequencerManager::SequencerManager():
    isSequenceRunning(false)
{
	__ConstructSingleton;
}

//------------------------------------------------------------------------------
/**
*/
SequencerManager::~SequencerManager()
{
    __DestructSingleton;
}

//------------------------------------------------------------------------------
/**
*/
void
SequencerManager::OnDeactivate()
{    
    int index;
    for(index = 0; index < this->inactiveSequencerPool.Size(); index++)
    {
        this->RemoveTrackSequencer(this->inactiveSequencerPool[index]);
    }
    for(index = 0; index < this->activeSequencerPool.Size(); index++)
    {
        this->RemoveTrackSequencer(this->activeSequencerPool[index]);
    }
    
    Manager::OnDeactivate();
}

//------------------------------------------------------------------------------
/**
*/
void
SequencerManager::OnStart()
{
	//empty
}

//------------------------------------------------------------------------------
/**
*/
void
SequencerManager::OnFrame()
{
    Manager::OnFrame();

    //update active track sequence
    int i;
    for(i = 0; i < this->activeSequencerPool.Size(); i++)
    {
        this->activeSequencerPool[i]->OnFrame();    
    }

    //check and set ist sequence runnig
    if(0 < this->activeSequencerPool.Size())
    {
        this->isSequenceRunning = true;
    }
    else
    {
        this->isSequenceRunning = false;
    }
}

//------------------------------------------------------------------------------
/**
*/
void
SequencerManager::AttachTrackSequencer(const Ptr<Sequencer::TrackSequencer>& sequencer)
{
    n_assert(sequencer.isvalid());
    n_assert2(  InvalidIndex == this->inactiveSequencerPool.FindIndex(sequencer) && 
                InvalidIndex == this->activeSequencerPool.FindIndex(sequencer), "Sequence allready added.");
    
    this->inactiveSequencerPool.Append(sequencer);
}

//------------------------------------------------------------------------------
/**
*/
void
SequencerManager::RemoveTrackSequencer(const Ptr<Sequencer::TrackSequencer>& sequencer)
{
    n_assert(sequencer.isvalid());
    sequencer->Discard();
    
    int index = this->activeSequencerPool.FindIndex(sequencer);
    //check if its a aktive sequence we have to remove
    if(InvalidIndex != index)
    {
        this->activeSequencerPool.Erase(this->activeSequencerPool.Find(sequencer));
    }
    //else its a inactive we have to remove
    else
    {
        this->inactiveSequencerPool.Erase(this->inactiveSequencerPool.Find(sequencer));
    }
}

//------------------------------------------------------------------------------
/**
*/
void
SequencerManager::AttachCutscene(const Ptr<Sequencer::TrackSequencer>& cutscene)
{
    n_assert(cutscene.isvalid());
    n_assert2(InvalidIndex == this->cutsceneSequencerPool.FindIndex(cutscene), "Cutscene allready added.");
    this->cutsceneSequencerPool.Append(cutscene);
}

//------------------------------------------------------------------------------
/**
*/
void
SequencerManager::RemoveCutscene(const Ptr<Sequencer::TrackSequencer>& cutscene)
{
    n_assert(cutscene.isvalid());
    IndexT index = this->cutsceneSequencerPool.FindIndex(cutscene);
    n_assert2(InvalidIndex != index, "Cutscene not found.");
    this->cutsceneSequencerPool.EraseIndex(index);
}

}//End namespace Sequencer