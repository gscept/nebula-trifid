//------------------------------------------------------------------------------
//  rocketserver.cc
//  (C) 2013 Gustav Sterbrant
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "rocketserver.h"
#include "Rocket/Core.h"
#include "Rocket/Controls.h"
#include "coregraphics/displaydevice.h"
#include "Rocket/Debugger.h"
#include "Rocket/Core/Input.h"
#include "io/uri.h"

using namespace Input;
using namespace CoreGraphics;
namespace LibRocket
{
__ImplementClass(LibRocket::RocketServer, 'ROSR', Base::UiServerBase);
__ImplementSingleton(LibRocket::RocketServer);

//------------------------------------------------------------------------------
/**
*/
static 
Rocket::Core::Font::Style 
UIToRocket(UI::FontStyle style)
{
	switch (style)
	{
	case UI::NormalStyle:
		return Rocket::Core::Font::STYLE_NORMAL;
	case UI::ItalicStyle:
		return Rocket::Core::Font::STYLE_ITALIC;
	default:
		return Rocket::Core::Font::STYLE_NORMAL;
	}
}

//------------------------------------------------------------------------------
/**
*/
static
Rocket::Core::Font::Weight
UIToRocket(UI::FontWeight style)
{
	switch (style)
	{
	case UI::NormalWeight:
		return Rocket::Core::Font::WEIGHT_NORMAL;
	case UI::BoldWeight:
		return Rocket::Core::Font::WEIGHT_BOLD;
	default:
		return Rocket::Core::Font::WEIGHT_NORMAL;
	}
}

//------------------------------------------------------------------------------
/**
*/
RocketServer::RocketServer() :
	context(0)
{
	__ConstructSingleton;
}

//------------------------------------------------------------------------------
/**
*/
RocketServer::~RocketServer()
{
	__DestructSingleton;
}

//------------------------------------------------------------------------------
/**
*/
void 
RocketServer::Setup()
{
	// setup base class
	UiServerBase::Setup();

	// set in Rocket
	Rocket::Core::SetSystemInterface(&this->system);
	Rocket::Core::SetRenderInterface(&this->renderer);    

	// initialize Rocket
	Rocket::Core::Initialise();
	Rocket::Controls::Initialise();

	Ptr<DisplayDevice> display = DisplayDevice::Instance();
	DisplayMode mode = display->GetDisplayMode();
	Rocket::Core::Vector2i dimensions(mode.GetWidth(), mode.GetHeight());
	this->context = Rocket::Core::CreateContext("main", dimensions);

#if !PUBLIC_BUILD
	Rocket::Debugger::Initialise(context);	
#endif	

	// create new listener
	this->listenerInstancer = new RocketEventListenerInstancer;

	// register event listener instancer
	Rocket::Core::Factory::RegisterEventListenerInstancer(this->listenerInstancer);
}

//------------------------------------------------------------------------------
/**
*/
void 
RocketServer::Discard()
{
	// shut down base class
	UiServerBase::Discard();

	this->listenerInstancer->RemoveReference();
	this->listenerInstancer = 0;

	this->context->RemoveReference();
	this->context = 0;
	Rocket::Core::Shutdown();
}

//------------------------------------------------------------------------------
/**
*/
bool 
RocketServer::IsValid() const
{
	return this->context != 0;
}

//------------------------------------------------------------------------------
/**
*/
void 
RocketServer::Update()
{
	n_assert(this->context);

	// update context
	this->context->Update();

	// run base class
	//UiServerBase::Update();
}

//------------------------------------------------------------------------------
/**
*/
void 
RocketServer::Render( const Ptr<Frame::FrameBatch>& frameBatch )
{
	n_assert(this->context);
	
    // render context
    this->context->Render();

    // not required to run render on layouts, context will deal with that
	// run base class
	//UiServerBase::Render(frameBatch);	
}

//------------------------------------------------------------------------------
/**
	Handle input through LibRocket.
	Consumes mouse button down and scroll events.
*/
bool 
RocketServer::HandleInput( const Input::InputEvent& event )
{
    switch (event.GetType())
    {        
    case InputEvent::KeyDown:
        this->context->ProcessKeyDown((Rocket::Core::Input::KeyIdentifier)Input::Key::ToRocket(event.GetKey()), 0);
		break;
    case InputEvent::KeyUp:
        this->context->ProcessKeyUp((Rocket::Core::Input::KeyIdentifier)Input::Key::ToRocket(event.GetKey()), 0);
        break;
    case InputEvent::Character:
        {
            char c = event.GetChar();

            // ignore backspace as a character
            if (c != 8)
            {				
                this->context->ProcessTextInput(event.GetChar());
            }
            break;
        }
	case InputEvent::MouseMove:
		this->context->ProcessMouseMove((int)event.GetAbsMousePos().x(), (int)event.GetAbsMousePos().y(), 0);
		break;
    case InputEvent::MouseButtonDown:
        this->context->ProcessMouseButtonDown(event.GetMouseButton(), 0);
        return this->context->GetHoverElement() != this->context->GetRootElement();
    case InputEvent::MouseButtonUp:
        this->context->ProcessMouseButtonUp(event.GetMouseButton(), 0);
        break;    
    case InputEvent::MouseWheelForward:
        this->context->ProcessMouseWheel(-1, 0);
        return this->context->GetHoverElement() != this->context->GetRootElement();
    case InputEvent::MouseWheelBackward:
        this->context->ProcessMouseWheel(1, 0);
        return this->context->GetHoverElement() != this->context->GetRootElement();
    }

	// this determines if we are hovering over the UI
	return false;
}

//------------------------------------------------------------------------------
/**
*/
void
RocketServer::LoadFont(const Util::String& resource, const Util::String& family, UI::FontStyle style, UI::FontWeight weight)
{
	IO::URI font(resource);
	Rocket::Core::FontDatabase::LoadFontFace(font.GetHostAndLocalPath().AsCharPtr(), family.AsCharPtr(), UIToRocket(style), UIToRocket(weight));
}

//------------------------------------------------------------------------------
/**
*/
void
RocketServer::LoadFont(const Util::String& resource)
{
	IO::URI font(resource);
	Rocket::Core::FontDatabase::LoadFontFace(font.GetHostAndLocalPath().AsCharPtr());
}

//------------------------------------------------------------------------------
/**
*/
void
RocketServer::LoadCursor(const Util::String& resource)
{
	n_assert(this->context);
	IO::URI doc(resource);

	Rocket::Core::ElementDocument* cursorDoc = this->context->LoadMouseCursor(doc.GetHostAndLocalPath().AsCharPtr());
	n_assert2(cursorDoc != NULL, "Failed to load cursor document");
	cursorDoc->RemoveReference();
}

//------------------------------------------------------------------------------
/**
*/
void
RocketServer::SetCursor(const Util::String& name)
{
	n_assert(this->context);
	
	bool success = this->context->SetMouseCursor(name.AsCharPtr());
	n_assert2(success, "Unknown mouse cursor");	
}

//------------------------------------------------------------------------------
/**
*/
void
RocketServer::SetDefaultCursor(const Util::String& name)
{
    n_assert(this->context);

    bool success = this->context->SetDefaultMouseCursor(name.AsCharPtr());
    n_assert2(success, "Unknown mouse cursor");	
}

//------------------------------------------------------------------------------
/**
*/
void
RocketServer::ShowCursor(bool visible)
{
	n_assert(this->context);
	this->context->ShowMouseCursor(visible);	
}

//------------------------------------------------------------------------------
/**
*/
void 
RocketServer::Resize(SizeT width, SizeT height)
{
	n_assert(this->context);

	Rocket::Core::Vector2i dimensions(width, height);
	this->context->SetDimensions(dimensions);
}

#if !PUBLIC_BUILD
//------------------------------------------------------------------------------
/**
*/
void 
RocketServer::SetRenderDebug( bool b )
{
	Rocket::Debugger::SetVisible(b);
}
#endif

} // namespace Rocket