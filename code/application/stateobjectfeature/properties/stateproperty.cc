//------------------------------------------------------------------------------
//  stateproperty.cc
//  (C) 2008 Radon Labs GmbH
//  (C) 2013-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "stateobjectfeature/properties/stateproperty.h"
#include "basegamefeature/basegameprotocol.h"
#include "io/ioserver.h"
#include "input/keyboard.h"
#include "input/inputserver.h"

const float MAX_TRIGGER_RADIUS = 10000.0f;

namespace StateObjectFeature
{
__ImplementClass(StateObjectFeature::StateProperty, 'STPR', Game::Property);

using namespace Game;
using namespace Util;

//------------------------------------------------------------------------------
/**
*/
StateProperty::StateProperty() :
    transitionStateActive(false),
    stateStartedTime(0.0f),
    originalTriggerRadius(0.0f)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
StateProperty::OnDeactivate()
{
    Property::OnDeactivate();
}

//------------------------------------------------------------------------------
/**
*/
void
StateProperty::SetupCallbacks()
{
    this->entity->RegisterPropertyCallback(this, BeginFrame);
}

//------------------------------------------------------------------------------
/**
*/
void
StateProperty::SetupDefaultAttributes()
{
    SetupAttr(Attr::State, false);
	SetupAttr(Attr::StateBaseTransform, false);
	SetupAttr(Attr::StateBaseTransformSet, false);
	SetupAttr(Attr::EntityTriggerRadius, false);
	SetupAttr(Attr::StateFilename, false);
    Property::SetupDefaultAttributes();
}

//------------------------------------------------------------------------------
/**
*/
void
StateProperty::OnLoad()
{
    Property::OnLoad();

    this->originalTriggerRadius = this->GetEntity()->GetFloat(Attr::EntityTriggerRadius);

    if (!this->GetEntity()->GetBool(Attr::StateBaseTransformSet))
    {
        // save position and rotation
        this->GetEntity()->SetMatrix44(Attr::StateBaseTransform, this->GetEntity()->GetMatrix44(Attr::Transform));
        this->GetEntity()->SetBool(Attr::StateBaseTransformSet, true);
    }
    this->baseTransform = this->GetEntity()->GetMatrix44(Attr::StateBaseTransform);

    this->ParseFile();

    Ptr<StateObjectFeature::Resources> message = StateObjectFeature::Resources::Create();
    message->SetResources(this->states);
    this->GetEntity()->SendSync(message.cast<Messaging::Message>());

    // read the State attribute and send a SwitchActiveState message to self
    const Util::String& curState = this->entity->GetString(Attr::State);
    Ptr<SwitchActiveState> msg = SwitchActiveState::Create();
    msg->SetStateName(curState);
    this->entity->SendSync(msg.cast<Messaging::Message>());
}

//------------------------------------------------------------------------------
/**
*/
void
StateProperty::ParseFile()
{
	// first open the file
    this->fileName = this->GetEntity()->GetString(Attr::StateFilename);
    Ptr<IO::Stream> stream = IO::IoServer::Instance()->CreateStream(IO::URI("export:" + this->fileName));

	// open xml 
	Ptr<IO::XmlReader> xml = IO::XmlReader::Create();
	xml->SetStream(stream);	
	if (!xml->Open())
    {
        n_error("Can not open stateobjectfile: %s", stream->GetURI().AsString().AsCharPtr());
    }

	// check root node
	String nodeName = xml->GetCurrentNodeName();
	n_assert("Nebula3" == nodeName);

	// get next node.. has to be a window node
	if(xml->SetToFirstChild("StateObject"))
	{
        n_assert(xml->GetCurrentNodeName() == "StateObject");
        xml->SetToFirstChild();
		do
		{
			this->ParseStateNode(xml);
		}while(xml->SetToNextChild());
	}
}

//------------------------------------------------------------------------------
/**
*/
void
StateProperty::ParseStateNode(Ptr<IO::XmlReader>& xml)
{
    n_assert(xml->GetCurrentNodeName() == "State");

	// parse node
	String name = xml->GetString("name");
    float transitionTime = 0.0f;
    if(xml->HasAttr("transitiontime"))
    {
        transitionTime = xml->GetString("transitiontime").AsFloat();
    }
    Util::String sound = "";
    if(xml->HasAttr("sound"))
    {
        sound = xml->GetString("sound");
    }
	
	// now parse the elements in the State node
	// first has to be a canvas
	if (xml->SetToFirstChild())
    {
	    this->ParseTransitionNode(xml, name, transitionTime, sound);
        xml->SetToParent();
    }
    else
    {
        StateInfo stateInfo;
        stateInfo.soundName = sound;
        stateInfo.stateDuration = transitionTime;

        this->states.Add(name, stateInfo);
    }
}

//------------------------------------------------------------------------------
/**
*/
void
StateProperty::ParseTransitionNode(Ptr<IO::XmlReader>& xml, Util::String name, float transitionTime, Util::String sound)
{
	n_assert(xml->GetCurrentNodeName() == "Transition");
    String transitionName = xml->GetString("name");
    String targetState = xml->GetString("targetstate");

    StateInfo stateInfo;
    stateInfo.soundName = sound;
    stateInfo.stateDuration = transitionTime;

    stateInfo.transitions.Add(targetState, transitionName);
    this->states.Add(name, stateInfo);
}

//------------------------------------------------------------------------------
/**
*/
void
StateProperty::OnSave()
{
    // save next state if in tansition 
    if (this->transitionStateActive && this->nextState.IsValid())
    {
        this->entity->SetString(Attr::State, this->nextState);
    }
    else
    {
        this->entity->SetString(Attr::State, this->curState);
    }
    this->GetEntity()->SetMatrix44(Attr::StateBaseTransform, this->baseTransform);
}

//------------------------------------------------------------------------------
/**
*/
void
StateProperty::SetupAcceptedMessages()
{
    this->RegisterMessage(StateObjectFeature::SwitchActiveState::Id);
    this->RegisterMessage(StateObjectFeature::GetActiveState::Id);
    this->RegisterMessage(StateObjectFeature::ContainsState::Id);
    this->RegisterMessage(BaseGameFeature::SetTransform::Id);
    Property::SetupAcceptedMessages();
}

//------------------------------------------------------------------------------
/**
*/
void
StateProperty::HandleMessage(const Ptr<Messaging::Message>& msg)
{
    if (msg->CheckId(StateObjectFeature::SwitchActiveState::Id))
    {
        this->OnSwitchActiveState(msg.cast<StateObjectFeature::SwitchActiveState>()->GetStateName());
    }
    else if (msg->CheckId(StateObjectFeature::GetActiveState::Id))
    {
        msg.cast<StateObjectFeature::GetActiveState>()->SetStateName(this->entity->GetString(Attr::State));
    }
    else if (msg->CheckId(StateObjectFeature::ContainsState::Id))
    {
        this->OnContainsState(msg);
    }
    else if (msg->CheckId(BaseGameFeature::SetTransform::Id))
    {
        Math::matrix44 transform = msg.cast<BaseGameFeature::SetTransform>()->GetMatrix();
        this->baseTransform = transform;
    }
    else
    {
        Property::HandleMessage(msg);
    }
}

//------------------------------------------------------------------------------
/**
    This checks whether we are in a transition state and whether we
    need to switch to the next state.
*/
void
StateProperty::OnBeginFrame()
{
    Ptr<Input::InputServer> inputServer = Input::InputServer::Instance();
    Ptr<Input::Keyboard> keyboard = inputServer->GetDefaultKeyboard();

    // do nothing if sleeping
    if (!this->GetEntity()->IsActive()) return;

    if (this->transitionStateActive)
    {
        const StateInfo& curStateInfo = this->states[this->curState];
        Timing::Time curTime = BaseGameFeature::GameTimeSource::Instance()->GetTime();
        if (curTime >= (this->stateStartedTime + curStateInfo.stateDuration))
        {
            // need to switch to next state
            Ptr<StateObjectFeature::SwitchActiveState> msg = StateObjectFeature::SwitchActiveState::Create();
            msg->SetStateName(this->nextState);
            this->GetEntity()->SendSync(msg.cast<Messaging::Message>());
        }
    }
}

//------------------------------------------------------------------------------
/**
    Called when a SwitchActiveState message is received. This will take
    care about state transitions, state sounds, and notifiying the
    graphics and physics state properties about the state change.
*/
void
StateProperty::OnSwitchActiveState(Util::String stateName)
{
    //n_assert(0 != msg);
    Util::String targetState = stateName;
    
    // make sure that if no transition state is set (several reasons) triggerraius is reset
    this->GetEntity()->SetFloat(Attr::EntityTriggerRadius, this->originalTriggerRadius);

    if (targetState != this->curState)
    {
        this->stateStartedTime = BaseGameFeature::GameTimeSource::Instance()->GetTime();
        this->transitionStateActive = false;
        this->nextState.Clear();

        // check if a state transition is defined
        if (this->curState.IsValid() && this->states.Contains(this->curState))
        {
            const StateInfo& curStateInfo = this->states[this->curState];
            if (curStateInfo.transitions.Contains(targetState))
            {
                // yes, there's a state transition defined, we need
                // to activate the transition state instead of the 
                // actual target state
                this->transitionStateActive = true;
                this->nextState = targetState;
                targetState = curStateInfo.transitions[targetState];
                this->originalTriggerRadius = this->GetEntity()->GetFloat(Attr::EntityTriggerRadius);
                this->GetEntity()->SetFloat(Attr::EntityTriggerRadius, MAX_TRIGGER_RADIUS);
            }
        }

        // handle target state stuff
        if (this->states.Contains(targetState))
        {
            this->curState = targetState;
            this->entity->SetString(Attr::State, targetState);

            // switch graphics and physics state
            Ptr<StateObjectFeature::InternalSwitchActiveState> internalSwitchStateMsg = StateObjectFeature::InternalSwitchActiveState::Create();
            internalSwitchStateMsg->SetStateName(targetState);
            this->entity->SendSync(internalSwitchStateMsg.cast<Messaging::Message>());

            // reset position
            if (!this->transitionStateActive)
            {
                // create and send the new position to entity
                Ptr<BaseGameFeature::SetTransform> setTransform = BaseGameFeature::SetTransform::Create();
                setTransform->SetMatrix(this->baseTransform);
                this->GetEntity()->SendSync(setTransform.cast<Messaging::Message>());
            }
        }
    }
}

//------------------------------------------------------------------------------
/**
    Called when a ContainsState message is received. This will set message result true
    if state given by message ist found in state list.
*/
void
StateProperty::OnContainsState(const Ptr<Messaging::Message>& msg)
{
    //n_assert(msg);
    Util::String targetState = msg.cast<StateObjectFeature::ContainsState>()->GetStateName();
    if (targetState.IsValid())
    {
        msg.cast<StateObjectFeature::ContainsState>()->SetResult(this->states.Contains(targetState));
        //msg->SetResult((this->states.Contains(targetState)));
    }
}

} // namespace StateObjectFeature
