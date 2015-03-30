#pragma once
//------------------------------------------------------------------------------
/**
    @class Rocket::RocketServer
    
    Implements a UI-server for LibRocket
    
    (C) 2013 Gustav Sterbrant
*/
//------------------------------------------------------------------------------
#include "ui/base/uiserverbase.h"
#include "rocketinterface.h"
#include "rocketrenderer.h"
#include "rocketeventinstancer.h"
#include "rocketeventlistenerinstancer.h"

namespace Rocket
{
namespace Core
{
	class Context;
}
}

namespace LibRocket
{
class RocketServer : public Base::UiServerBase
{
	__DeclareClass(RocketServer);
	__DeclareSingleton(RocketServer);
public:
	/// constructor
	RocketServer();
	/// destructor
	virtual ~RocketServer();

	/// setup the server
	void Setup();
	/// discard the server
	void Discard();
	/// returns true if the server is valid
	bool IsValid() const;
	/// updates context
	void Update();
	/// renders context
	void Render(const Ptr<Frame::FrameBatch>& frameBatch);
	/// load font
	void LoadFont(const Util::String& resource, const Util::String& family, UI::FontStyle style, UI::FontWeight weight);
	/// load font and try to autodetect information about it
	void LoadFont(const Util::String& resource);
	/// load mouse cursor, first loaded cursor becomes default
	void LoadCursor(const Util::String& resource);
	/// selects a previously loaded cursor, name is defined by document title
	void SetCursor(const Util::String & name);
    /// selects a previously loaded cursor as default cursor, name is defined by document title
    void SetDefaultCursor(const Util::String & name);
	/// sets cursor visibility
	void ShowCursor(bool visible);

	/// resize context
	void Resize(SizeT width, SizeT height);

#if !PUBLIC_BUILD
	/// starts debug rendering
	void SetRenderDebug(bool b);
#endif

    /// handle input event
    bool HandleInput(const Input::InputEvent& event);

	/// get rocket context
	Rocket::Core::Context* GetContext();

	friend class RocketLayout;

private:

	/// convenience function which converts an InputEvent key to 
	Rocket::Core::Context* context;
	RocketInterface system;
	RocketRenderer renderer;
	RocketEventListenerInstancer* listenerInstancer;
}; 


//------------------------------------------------------------------------------
/**
*/
inline Rocket::Core::Context* 
RocketServer::GetContext()
{
	return this->context;
}


} // namespace Rocket
//------------------------------------------------------------------------------