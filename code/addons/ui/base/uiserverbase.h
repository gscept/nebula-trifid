#pragma once
//------------------------------------------------------------------------------
/**
    @class Base::UiServerBase

    Central UI server on the render-thread-side.

    (C) 2009 Radon Labs GmbH
    (C) 2013-2016 Individual contributors, see AUTHORS file
*/
#include "core/refcounted.h"
#include "core/singleton.h"
#include "util/stringatom.h"
#include "resources/resourceid.h"
#include "ui/uilayout.h"
#include "ui/uievent.h"
#include "input/inputevent.h"
#include "timing/timer.h"
#include "threading/safequeue.h"

namespace UI
{
	enum FontStyle
	{
		NormalStyle = 0,
		ItalicStyle = 1
	};
	enum FontWeight
	{
		NormalWeight = 0,
		BoldWeight = 1
	};

}

//------------------------------------------------------------------------------
namespace Base
{
class UiServerBase : public Core::RefCounted
{
    __DeclareClass(UiServerBase);
    __DeclareSingleton(UiServerBase);
public:
    /// constructor
    UiServerBase();
    /// destructor
    virtual ~UiServerBase();

    /// setup the server
    void Setup();
    /// discard the server
    void Discard();
    /// return true if server has been setup
    bool IsValid() const;

    /// create a layout object
    Ptr<UI::UiLayout> CreateLayout(const Util::String& layoutId, const Resources::ResourceId& resId);
    /// get layout based on layout id
    const Ptr<UI::UiLayout>& GetLayoutById(const Util::String& layoutId);
	/// layout is loaded
	bool HasLayout(const Util::String& layoutId) const;
	/// get loaded layouts
	Util::Array<Util::String> GetLayouts();
	/// free a layout
	void FreeLayout(const Util::String& layoutId);
	/// resize context
	void Resize(SizeT width, SizeT height);

    /// handle input event
    bool HandleInput(const Input::InputEvent& event);
    /// update the UI system, called exactly once per frame
    void Update();
    /// render UI system, may be called several times per frame
    void Render(const Util::StringAtom& filter);
    /// get current UI time
    Timing::Time GetTime() const;
    /// get current UI time ticks
    Timing::Tick GetTicks() const;

	/// sets debug rendering
	void SetRenderDebug(bool b);

	/// Load font
	void LoadFont(const Util::String& resource, const Util::String& family, const Util::String& style, const Util::String& weight);
	/// load font and try to autodetect information about it
	void LoadFont(const Util::String& resource);


protected:
    Util::Dictionary<Util::String, Ptr<UI::UiLayout> > layouts;
    bool isValid;
    Timing::Timer timer;
    Timing::Time curTime;
    Timing::Tick curTicks;
    
#ifdef _DEBUG   
    struct msgElement
    {
        Util::StringAtom layoutId;
        Util::StringAtom elementId;
    };
    
    Util::Dictionary<Util::String, Util::Array<msgElement> > messagesPerFrame;

#endif
 
};

//------------------------------------------------------------------------------
/**
*/
inline bool
UiServerBase::IsValid() const
{
    return this->isValid;
}

//------------------------------------------------------------------------------
/**
*/
inline Timing::Time
UiServerBase::GetTime() const
{
    return this->curTime;
}

//------------------------------------------------------------------------------
/**
*/
inline Timing::Tick
UiServerBase::GetTicks() const
{
    return this->curTicks;
}

//------------------------------------------------------------------------------
/**
*/
inline void 
UiServerBase::SetRenderDebug( bool b )
{
	// override in subclass
}

//------------------------------------------------------------------------------
/**
*/
inline bool
UiServerBase::HasLayout(const Util::String& layoutId) const
{
	return this->layouts.Contains(layoutId);
}

} // namespace UI
//------------------------------------------------------------------------------
