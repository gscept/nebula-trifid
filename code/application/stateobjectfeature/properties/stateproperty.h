#pragma once
//------------------------------------------------------------------------------
/**
    @class StateObjectFeature::StateProperty

    Central property which manages "state switches". These are just 
    visual/physical state switches, not behavioral state switches.
    Usually works together with a StatePhysicsProperty and
    StateGraphicsProperty.

    FIXME: need to implement advanced stuff: Play Sound,
    Transition States.
    
    (C) 2008 Radon Labs GmbH
    (C) 2013-2015 Individual contributors, see AUTHORS file
*/
#include "game/property.h"
#include "attr/attributedefinition.h"
#include "util/dictionary.h"
#include "stateobjectfeature/stateobjectprotocol.h"
#include "basegametiming/gametimesource.h"
#include "io/xmlreader.h"
#include "io/filestream.h"
#include "application/stateobjectfeature/properties/stateinfo.h"
#include "game/entity.h"

//------------------------------------------------------------------------------
namespace StateObjectFeature
{
class StateProperty : public Game::Property
{
	__DeclareClass(StateProperty);
public:
    /// constructor
    StateProperty();
    /// setup callbacks for this property
    virtual void SetupCallbacks();
    /// setup default entity attributes
    virtual void SetupDefaultAttributes();
    /// called from within Entity::Load() after attributes are loaded
    virtual void OnLoad();
    /// called from within Entity::Save() before attributes are saved back to database
    virtual void OnSave();
    /// override to register accepted messages
    virtual void SetupAcceptedMessages();
    /// handle a single message
    virtual void HandleMessage(const Ptr<Messaging::Message>& msg);
    /// called on begin of frame
    virtual void OnBeginFrame();
    /// called from Entity::DeactivateProperties()
    virtual void OnDeactivate();
    /// sets the filename of the ui.xml file
	void SetFileName(const Util::String& name);
	/// gets the filename
	const Util::String& GetFileName() const;
    /// opens the file and creates the gui structure
	void ParseFile();

private:
    /// called on SwitchActiveState message
    void OnSwitchActiveState(Util::String stateName);
    /// called on ContainsState message
    void OnContainsState(const Ptr<Messaging::Message>& msg);

    /// parse the window node
	void ParseStateNode(Ptr<IO::XmlReader>& xml);
	/// parse the button node
    void ParseTransitionNode(Ptr<IO::XmlReader>& xml, Util::String name, float transitionTime, Util::String sound);


    Util::Dictionary<Util::String, StateInfo> states;
    bool transitionStateActive;
    Util::String curState;
    Util::String nextState;
    Timing::Time stateStartedTime;
    Math::matrix44 baseTransform;
    // remember EntityTriggerRadius, danger! danger!
    // no other property must do this.
    float originalTriggerRadius; 
    Util::String fileName;
};
__RegisterClass(StateProperty);

//------------------------------------------------------------------------------
/**
*/
inline
void
StateProperty::SetFileName(const Util::String& name)
{
    this->fileName = name;
}


//------------------------------------------------------------------------------
/**
*/
inline
const Util::String& 
StateProperty::GetFileName() const
{
	return this->fileName;
}

} // namespace StateObjectFeature
//------------------------------------------------------------------------------
