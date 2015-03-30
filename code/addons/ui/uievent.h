#pragma once
//------------------------------------------------------------------------------
/**
    @class UI::UiEvent

    An UIEvent defines what and where some action has happened in a
    layout (for instance, a button has been pressed).

    (C) 2009 Radon Labs GmbH
    (C) 2013-2015 Individual contributors, see AUTHORS file
*/
#include "core/types.h"
#include "util/stringatom.h"
#include "input/inputevent.h"
#include "threading/objectref.h"

//------------------------------------------------------------------------------
namespace UI
{
class UiEvent
{
public:
    /// event action enums
    enum Action
    {
        NoAction  = 0,
        MouseEvent,
		MouseEnter,
		MouseLeave,
		KeyEvent,
		WindowEvent,
		TextChanged,
		CheckChanged,
		ScrollChanged,
		ValueChanged,
    };

    /// default constructor
    UiEvent();
    /// constructor
    UiEvent(const Util::String& layout, const Util::String& eventName, const Util::String & eventScript, uint actions);
    /// equality operator (NOTE: "any match" is true for action bits)
    bool operator==(const UiEvent& rhs) const;
    /// inequality operator (NOTE: "no match" on action bits!)
    bool operator!=(const UiEvent& rhs) const;
    /// setup the event
	void Setup(const Util::String& layout, const Util::String& eventName, const Util::String & eventScript, uint actions);
    /// get layout id
    const Util::String& GetLayout() const;
    /// get element id
    const Util::String& GetEventName() const;
	/// get script to execute
	const Util::String& GetEventScript() const;
    /// get action mask
    uint GetActions() const;

private:
    Util::String layout;
    Util::String eventName;
	Util::String eventScript;
    uint actions;

};

//------------------------------------------------------------------------------
/**
*/
inline
UiEvent::UiEvent() :
    actions(0)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
UiEvent::UiEvent(const Util::String& lid, const Util::String& eid, const Util::String & es, uint actns) :
    layout(lid),
    eventName(eid),
	eventScript(es),
    actions(actns)    
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline bool
UiEvent::operator==(const UiEvent& rhs) const
{
    return (this->layout == rhs.layout) &&
           (this->eventName == rhs.eventName) &&
           ((this->actions & rhs.actions) != 0);
}

//------------------------------------------------------------------------------
/**
*/
inline bool
UiEvent::operator!=(const UiEvent& rhs) const
{
    return *this != rhs;
}

//------------------------------------------------------------------------------
/**
*/
inline const Util::String&
UiEvent::GetLayout() const
{
    return this->layout;
}

//------------------------------------------------------------------------------
/**
*/
inline void
UiEvent::Setup(const Util::String& lid, const Util::String& eid, const Util::String & es, uint actns)
{
    this->layout = lid;
    this->eventName = eid;
	this->eventScript = es;
    this->actions = actns;   
}

//------------------------------------------------------------------------------
/**
*/
inline const Util::String&
UiEvent::GetEventName() const
{
    return this->eventName;
}

//------------------------------------------------------------------------------
/**
*/
inline const Util::String&
UiEvent::GetEventScript() const
{
	return this->eventScript;
}
//------------------------------------------------------------------------------
/**
*/
inline uint
UiEvent::GetActions() const
{
    return this->actions;
}

} // namespace UI
//------------------------------------------------------------------------------
