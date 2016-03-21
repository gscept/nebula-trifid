//------------------------------------------------------------------------------
//  rocketeventlistener.cc
//  (C) 2013-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "memory/memory.h"
#include "rocketeventinstancer.h"
#include "util/array.h"
#include "rocketserver.h"
#include "scripting/scriptserver.h"
#include "ui/uievent.h"
#include "ui/uiserver.h"
#include "ui/uifeatureunit.h"

using namespace Util;
using namespace Scripting;
namespace LibRocket
{

//------------------------------------------------------------------------------
/**
*/
RocketEventInstancer::RocketEventInstancer()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
RocketEventInstancer::~RocketEventInstancer()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
Rocket::Core::Event* 
RocketEventInstancer::InstanceEvent( Rocket::Core::Element* target, 
									 const Rocket::Core::String& name, 
									 const Rocket::Core::Dictionary& parameters, 
									 bool interruptible )
{
	Ptr<RocketServer> rocketServer = RocketServer::Instance();
	String script = name.CString();
	Array<String> parts = script.Tokenize(".");

	// only handle our scripts
	if (parts.Size() == 2)
	{
		// first segment is the layout
		Ptr<RocketLayout> layout = rocketServer->GetLayoutById(parts[0]).downcast<RocketLayout>();

		String functionSignature;
		functionSignature.Format("%s(", parts[1].AsCharPtr());

		IndexT i;
		for (i = 0; i < parameters.Size(); i++)
		{
            Rocket::Core::String key;
            Rocket::Core::String value;
            parameters.Iterate<Rocket::Core::String>(i, key, value);
            functionSignature.Append(key.CString());
            functionSignature.Append(", ");
		}

        // remove any superflous comma
        functionSignature.TrimRight(",");
		functionSignature.Append(")");

		UI::UiEvent ev(parts[0],parts[1],functionSignature,UI::UiEvent::ValueChanged);
        UI::UiFeatureUnit::Instance()->ProcessEvent(ev);

		return NULL;
	}
	else
	{
		return Rocket::Core::EventInstancerDefault::InstanceEvent(target, name, parameters, interruptible);
	}
}

//------------------------------------------------------------------------------
/**
*/
void 
RocketEventInstancer::ReleaseEvent( Rocket::Core::Event* event )
{
	Rocket::Core::EventInstancerDefault::ReleaseEvent(event);
}

//------------------------------------------------------------------------------
/**
*/
void 
RocketEventInstancer::Release()
{
	Rocket::Core::EventInstancerDefault::Release();
}
} // namespace Rocket