//------------------------------------------------------------------------------
//  uilayoutbase.cc
//  (C) 2009 Radon Labs GmbH
//  (C) 2013-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "ui/base/uilayoutbase.h"
#include "ui/uiserver.h"
#include "io/ioserver.h"
#include "io/stream.h"

namespace Base
{
__ImplementClass(Base::UiLayoutBase, 'SULB', Core::RefCounted);

using namespace Util;
using namespace UI;
using namespace Input;
using namespace Resources;

//------------------------------------------------------------------------------
/**
*/
UiLayoutBase::UiLayoutBase() :
    isValid(false),
    resState(Resource::Initial),
    showTimeStamp(0.0),
    hideTimeStamp(0.0),
    shown(false)
    
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
UiLayoutBase::~UiLayoutBase()
{
    n_assert(!this->IsValid());
}

//------------------------------------------------------------------------------
/**
*/
void
UiLayoutBase::Setup(const StringAtom& lid, const StringAtom& resId)
{
    n_assert(!this->IsValid());
    this->layoutId = lid;
    this->resourceId = resId;
    this->shown = false;
    this->isValid = true;

    // load script
    IO::URI script(resId.AsString());
    Util::String file = script.LocalPath();
    file.ChangeFileExtension("lua");

    // create stream to file
    Ptr<IO::Stream> scriptStream = IO::IoServer::Instance()->CreateStream(file);

    // only load script if stream can be opened
    if (scriptStream->Open())
    {
        // read data
        void* data = scriptStream->Map();
        SizeT size = scriptStream->GetSize();

        // set script
        this->script.AppendRange((const char*)data, size);
    }
}

//------------------------------------------------------------------------------
/**
*/
void
UiLayoutBase::Discard()
{
    n_assert(this->IsValid());
    this->isValid = false;

	IndexT i;
	for (i = 0; i < this->elements.Size(); i++)
	{
		this->elements.ValueAtIndex(i)->Discard();
	}
	this->elements.Clear();
}

//------------------------------------------------------------------------------
/**
*/
Ptr<UI::UiElement> 
UiLayoutBase::GetElement( const Util::StringAtom& elementId )
{
    if (this->elements.Contains(elementId))
    {
        return this->elements[elementId];
    }
    else
    {
        // setup UI element the first time we get it
        Ptr<UI::UiElement> element = UI::UiElement::Create();
        element->SetElementId(elementId);

		Ptr<Base::UiLayoutBase> thisPtr(this);
		element->Setup(thisPtr.downcast<UI::UiLayout>());
        
        // add it to local dictionary
        this->elements.Add(elementId, element);
        return element;
    }
}

//------------------------------------------------------------------------------
/**
    Call this method to make the layout visible.
*/
void
UiLayoutBase::Show()
{
    n_assert(this->IsValid());
    if (!this->shown)
    {
        this->shown = true;
        this->showTimeStamp = UiServer::Instance()->GetTime();        
    }
}

//------------------------------------------------------------------------------
/**
    Call this method to make the layout invisible.
*/
void
UiLayoutBase::Hide()
{
    n_assert(this->IsValid());
    if (this->shown)
    {
        this->shown = false;
        this->hideTimeStamp = UiServer::Instance()->GetTime();
    }
}

//------------------------------------------------------------------------------
/**
*/
void 
UiLayoutBase::Toggle()
{
    n_assert(this->IsValid());
    if (this->shown)
    {
        this->Hide();
    }
    else
    {
        this->Show();
    }
}

//------------------------------------------------------------------------------
/**
    This method returns true if the layout is currently in the shown state.
*/
bool
UiLayoutBase::IsShown() const
{
    return this->shown;
}

//------------------------------------------------------------------------------
/**
    Start or continue loading the layout resource. Loading should happen
    asynchronously. As long as the IsPending() method returns true, the
    app should continue to call Load() until IsPending() returns false.
    If loading has failed, the LoadFailed() method will return true.
*/
void
UiLayoutBase::Load()
{
    // override in subclass!
}

//------------------------------------------------------------------------------
/**
*/
bool
UiLayoutBase::IsPending() const
{
    return Resource::Pending == this->resState;
}

//------------------------------------------------------------------------------
/**
*/
bool
UiLayoutBase::LoadFailed() const
{
    return Resource::Failed == this->resState;
}

//------------------------------------------------------------------------------
/**
*/
void 
UiLayoutBase::Reload()
{
	IndexT i;
	for (i = 0; i < this->elements.Size(); i++)
	{
		this->elements.ValueAtIndex(i)->Discard();
	}
	this->elements.Clear();
}

//------------------------------------------------------------------------------
/**
    This method must be overwritten in a subclass and return true as soon
    as the layout is ready to be displayed.
*/
bool
UiLayoutBase::IsShowable() const
{
    return Resource::Loaded == this->resState;
}

//------------------------------------------------------------------------------
/**
    This method takes an array of input events, and produce a resulting 
    UIEvents which must be appended(!) to the outEvents array. The method
    must return the number of produced UIEvents.
*/
SizeT
UiLayoutBase::ProcessInputEvents(const Array<InputEvent>& inputEvents)
{
    return 0;
}

//------------------------------------------------------------------------------
/**
*/
void
UiLayoutBase::HandleInput(const Input::InputEvent& event)
{
    // override in subclass
}

//------------------------------------------------------------------------------
/**
    This method is called once per frame on ALL layouts (visible or not), and
    is guaranteed to be only called once per frame. This is where layout
    animation and other dynamic effects should happen.
*/
void
UiLayoutBase::Update()
{
    // override in subclass!
}

//------------------------------------------------------------------------------
/**
    This method is called when the layout should render itself. The method
    may be called several times per frame.
    NOTE that the method will even be called, when the layout is not currently
    set to shown state. The method must decide on its own whether it wants
    to render itself.
*/
void
UiLayoutBase::Render(const Util::StringAtom& filter)
{
    // override in subclass!
}

} // namespace Base