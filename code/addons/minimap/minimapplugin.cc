//------------------------------------------------------------------------------
//  minimapplugin.cc
//  (C) 2015 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "minimapplugin.h"
#include "resources/resourceid.h"
#include "resources/resourcemanager.h"
#include "minimapmanager.h"
#include "uifeatureunit.h"

using namespace Util;
using namespace CoreGraphics;
using namespace Resources;
using namespace Math;
using namespace Models;

namespace Minimap
{
    __ImplementClass(MinimapPlugin, 'MIMP', UI::UiPlugin);

//------------------------------------------------------------------------------
/**
*/
MinimapPlugin::MinimapPlugin()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
MinimapPlugin::~MinimapPlugin()
{
	this->RemoveReference();
}

//------------------------------------------------------------------------------
/**
*/
Rocket::Core::EventListener*
MinimapPlugin::InstanceEventListener(const Rocket::Core::String& value, Rocket::Core::Element* element)
{
	if (element->GetTagName() == "minimap")
	{
        this->eventValue = value.CString();
		return this;
	}
	return 0;
}

//------------------------------------------------------------------------------
/**
*/
void
MinimapPlugin::ProcessEvent(Rocket::Core::Event& event)
{
	if (event.GetType() == "click" && !this->eventValue.IsEmpty())
	{
		float x = (float)event.GetParameter<int>("mouse_x", 0);
		float y = (float)event.GetParameter<int>("mouse_y", 0);
        Rocket::Core::Vector2f offset = event.GetCurrentElement()->GetAbsoluteOffset(Rocket::Core::Box::CONTENT);
        float width = event.GetCurrentElement()->GetClientWidth();
        float height = event.GetCurrentElement()->GetClientHeight();
        
        x -= offset.x;
        y -= offset.y;        
        x /= width;
        y /= height;        

        Util::String format;
        format.Format("%s(%f,%f)", this->eventValue.AsCharPtr(), x, y);
        // translate to UIEvent and add event to server
        UI::UiEvent ev("", "minimap", format, UI::UiEvent::ValueChanged);
        UI::UiFeatureUnit::Instance()->ProcessEvent(ev);
    }	
}

//------------------------------------------------------------------------------
/**
*/
void
MinimapPlugin::Release()
{

}

//------------------------------------------------------------------------------
/**
*/
void
MinimapPlugin::OnRegister()
{
    LibRocket::RocketServer::Instance()->AddEventListenerInstancer(this);
	Rocket::Core::StyleSheetSpecification::RegisterProperty("minimaptexture", "", false).AddParser("string");
	Rocket::Core::StyleSheetSpecification::RegisterProperty("minimapoverlay", "", false).AddParser("string");
	Rocket::Core::StyleSheetSpecification::RegisterProperty("minimapoffset", "0", false).AddParser("number");
}

//------------------------------------------------------------------------------
/**
*/
void
MinimapPlugin::OnUnregister()
{
	//empty
}

} // namespace Minimap

