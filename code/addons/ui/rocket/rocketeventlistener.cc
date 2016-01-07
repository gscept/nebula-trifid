//------------------------------------------------------------------------------
//  rocketevent.cc
//  (C) 2013-2015 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "memory/memory.h"
#include "rocketeventlistener.h"
#include "util/array.h"
#include "rocketserver.h"
#include "scripting/scriptserver.h"
#include "ui/uievent.h"
#include "ui/uifeatureunit.h"
#include "Rocket/Controls/ElementFormControl.h"
#include "faudio/audiodevice.h"
#include "faudio/eventid.h"

using namespace Util;
using namespace Scripting;
namespace LibRocket
{

//------------------------------------------------------------------------------
/**
*/
RocketEventListener::RocketEventListener(const Rocket::Core::String & val)
{
	// get script function to run whenever this listener gets invoked
	String script = val.CString();
	this->eventParameter = "";

	// split string into parts
	Array<String> parts = script.Tokenize(".");
	n_assert(parts.Size() == 2);
	this->layoutId = parts[0];
	this->eventName = parts[1];
	String params = parts[1];
	int leftparenthesis = params.FindCharIndex('(');
	int rightparenthesis= params.FindCharIndex(')');
	if (InvalidIndex != leftparenthesis)
	{
		if (InvalidIndex != rightparenthesis)
		{
			this->eventParameter = params.ExtractRange(leftparenthesis + 1, rightparenthesis-leftparenthesis - 1);
			this->eventName = params.ExtractRange(0, leftparenthesis);
		}
	}	
}

//------------------------------------------------------------------------------
/**
*/
RocketEventListener::~RocketEventListener()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
void
RocketEventListener::ProcessEvent(Rocket::Core::Event& event)
{
	// check for sound event
	if (event.GetType() == "click")
	{
		Rocket::Core::Element * elem = event.GetTargetElement();
		const Rocket::Core::Property * property = elem->GetProperty("click-sound");
		if (property)
		{
			FAudio::EventId sound = property->ToString().CString();
			if (FAudio::AudioDevice::HasInstance())
			{
				FAudio::AudioDevice::Instance()->EventPlayFireAndForget(sound, 1.0f);
			}
		}
	}

	Util::String functionSignature;	
	if (event.GetType() == "change")
	{
		Util::String value;
		Rocket::Controls::ElementFormControl * elem = (Rocket::Controls::ElementFormControl*) event.GetTargetElement();
		Util::String elemtype = elem->GetAttribute<Rocket::Core::String>("type", "").CString();
		if (elemtype == "text")
		{		
			bool lf = event.GetParameter<bool>("linebreak", false);
			
			value.Format("\"%s\",%s", event.GetParameter<Rocket::Core::String>("value", "").CString(),lf?"true":"false");			
		}
		else if (elemtype == "checkbox")
		{
			if (elem->HasAttribute("checked"))
			{
				value = "true";
			}
			else
			{
				value = "false";
			}
		}
		else
		{
			value = elem->GetValue().CString();
		}
		if (this->eventParameter == "")
		{
			functionSignature.Format("%s(%s)", this->eventName.AsCharPtr(),value.AsCharPtr());
		}
		else
		{
			functionSignature.Format("%s(%s,%s)", this->eventName.AsCharPtr(), this->eventParameter.AsCharPtr(), value.AsCharPtr());
		}
	}
	else if(event.GetType() =="keydown")
	{
		int val = event.GetParameter<int>("key_identifier",-1);
		val = Input::Key::FromRocket(val);
		functionSignature.Format("%s(%d)", this->eventName.AsCharPtr(), val);
	}
	else if(event.GetType() =="mouseup")
	{
		int val = event.GetParameter<int>("button",-1);
		functionSignature.Format("%s(%d)", this->eventName.AsCharPtr(), val);
	}
	else
	{		
		if (this->eventParameter == "")
		{
			functionSignature.Format("%s()", this->eventName.AsCharPtr());
		}
		else
		{
			functionSignature.Format("%s(%s)", this->eventName.AsCharPtr(), this->eventParameter.AsCharPtr());
		}
	}

	// translate to UIEvent and add event to server
	UI::UiEvent ev(this->layoutId, this->eventName, functionSignature, UI::UiEvent::ValueChanged);
    UI::UiFeatureUnit::Instance()->ProcessEvent(ev);
}

}


