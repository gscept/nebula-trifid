//------------------------------------------------------------------------------
//  behaviourproperty.cc
//  (C) 2006 Radon Labs GmbH
//  (C) 2013-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "scriptfeature/fsm/behaviourproperty.h"
#include "scriptfeature/fsm/statemachine.h"
#include "scriptfeature/fsm/state.h"
#include "scriptfeature/managers/scriptmanager.h"
#include "scriptfeature/scriptprotocol.h"
#include "game/entity.h"
#include "debugrender/debugrender.h"
#include "basegamefeature/basegameattr/basegameattributes.h"
#include "basegamefeature/basegametiming/gametimesource.h"
#include "scriptfeature/actions/sequenceaction.h"
#include "scriptfeature/actions/actionlist.h"

namespace FSM
{
__ImplementClass(FSM::BehaviourProperty, 'BEPR', Game::Property);

using namespace Util;
using namespace Math;


int BehaviourProperty::numStateMachines = 0;
Timing::Time BehaviourProperty::updateTime = 0.25;
//------------------------------------------------------------------------------
/**
*/
void
BehaviourProperty::SetupCallbacks()
{
    this->entity->RegisterPropertyCallback(this, BeginFrame);
    this->entity->RegisterPropertyCallback(this, RenderDebug);
}

//------------------------------------------------------------------------------
/**
*/
void
BehaviourProperty::OnActivate()
{
    Property::OnActivate();

    // if a behaviour is defined, load it into a statemachine
    const String& behaviour = this->GetEntity()->GetString(Attr::Behaviour);
    if (behaviour.IsValid())
    {
        // load stateMachine from ScriptManager
        this->stateMachine = Script::ScriptManager::Instance()->LoadStateMachine(behaviour, this->GetEntity());
        n_assert(this->stateMachine.isvalid());
        this->stateMachine->Open();

        this->stateMachineIndex = BehaviourProperty::numStateMachines;
        BehaviourProperty::numStateMachines++;
    }
}

//------------------------------------------------------------------------------
/**
*/
void
BehaviourProperty::OnDeactivate()
{
    //clear the state machine
    if (this->stateMachine.isvalid())
    {
        this->stateMachine->Close();
        this->stateMachine = 0;
    }
    Property::OnDeactivate();
}

//------------------------------------------------------------------------------
/**
*/
void
BehaviourProperty::OnBeginFrame()
{
    // trigger the state machine
    if (this->stateMachine.isvalid())
    {
        // check if our statemachine is in his timeslot
        /*Timing::Time timeSlot = updateTime / (Timing::Time)numStateMachines;
        Timing::Time curTime = BaseGameFeature::GameTimeSource::Instance()->GetTime();
        float rest = fmodf((float)curTime, (float)updateTime);
        if (updateTime == 0.0 ||
            (rest > timeSlot * this->stateMachineIndex &&
            rest < timeSlot * (this->stateMachineIndex + 1)))
        {*/
            this->stateMachine->Update();
        //}        
    }
}

//------------------------------------------------------------------------------
/**
*/
void 
BehaviourProperty::OnRenderDebug()
{
 //   // show current state
 //   const Ptr<FSM::State> state = this->stateMachine->GetCurrentState();
 //   Util::String stateName = "State: " + state->GetName();
 //   point entityPos = GetEntity()->GetMatrix44(Attr::Transform).get_position() + vector(0,2,0);
 //   // show cur state 
 //   _debug_text3D(stateName, entityPos, point(1,1,0));    
 //   const Ptr<Actions::SequenceAction>& actions = state->GetFrameActions();
 //   String allActions;    
	//Util::Array<Ptr<Actions::Action> > list = actions->GetActionList();    
 //   while (list.Size() > 0 && list[0]->IsA(Actions::ActionList::RTTI))
 //   {
 //       list = list[0].cast<Actions::ActionList>()->GetActionList();
 //   }
 //   IndexT actionIdx;
 //   for (actionIdx = 0; actionIdx < list.Size(); ++actionIdx)
 //   {
 //       allActions.Append(list[actionIdx]->GetClassName());
 //       allActions.Append("; ");
 //   }
 //   
 //   _debug_text3D(allActions, entityPos - vector(0,0.2,0), point(1,1,0));

}

//------------------------------------------------------------------------------
/**
*/
void 
BehaviourProperty::SetupAcceptedMessages()
{
    this->RegisterMessage(ScriptFeature::SwitchFSM::Id);
    this->RegisterMessage(ScriptFeature::GetCurrentState::Id);
    Game::Property::SetupAcceptedMessages();
}

//------------------------------------------------------------------------------
/**
*/
void 
BehaviourProperty::HandleMessage(const Ptr<Messaging::Message>& msg)
{
    if (msg->CheckId(ScriptFeature::SwitchFSM::Id))
    { 
        this->SwitchStateMachine(msg.cast<ScriptFeature::SwitchFSM>()->GetStateMachineName());
    }
    else if (msg->CheckId(ScriptFeature::GetCurrentState::Id))
    {         
        if (this->stateMachine.isvalid() && this->stateMachine->GetCurrentState().isvalid())
        {
            Util::String stateName = this->stateMachine->GetCurrentState()->GetName();
            msg.cast<ScriptFeature::GetCurrentState>()->SetStateName(stateName);
        }                
    }
    else
    {
        Game::Property::HandleMessage(msg);
    }
}

//------------------------------------------------------------------------------
/**
*/
void 
BehaviourProperty::SwitchStateMachine(const Util::String& stateMachineName)
{
    n_assert(stateMachineName.IsValid());
    if (this->stateMachine.isvalid() && this->stateMachine->IsOpen())
    {
        this->stateMachine->Close();
    }

    this->stateMachine = Script::ScriptManager::Instance()->LoadStateMachine(stateMachineName, this->GetEntity());
    n_assert(this->stateMachine.isvalid());
    this->stateMachine->Open();
}
} // namespace Properties