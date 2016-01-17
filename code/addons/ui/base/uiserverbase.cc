//------------------------------------------------------------------------------
//  uiserverbase.cc
//  (C) 2009 Radon Labs GmbH
//  (C) 2013-2015 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "ui/base/uiserverbase.h"

namespace Base
{
__ImplementClass(Base::UiServerBase, 'SUSB', Core::RefCounted);
__ImplementSingleton(Base::UiServerBase);

using namespace Util;
using namespace Resources;
using namespace UI;
using namespace Input;

//------------------------------------------------------------------------------
/**
*/
UiServerBase::UiServerBase() :
    isValid(false),
    curTime(0.0),
    curTicks(0)
{
    __ConstructSingleton;
}

//------------------------------------------------------------------------------
/**
*/
UiServerBase::~UiServerBase()
{
    n_assert(!this->IsValid());
    __DestructSingleton;
}

//------------------------------------------------------------------------------
/**
*/
void
UiServerBase::Setup()
{
    n_assert(!this->IsValid());
    this->isValid = true;
}

//------------------------------------------------------------------------------
/**
*/
void
UiServerBase::Discard()
{
    n_assert(this->IsValid());

    // discard existing layouts
    IndexT i;
    for (i = 0; i < this->layouts.Size(); i++)
    {
        this->layouts.ValueAtIndex(i)->Discard();
    }
    this->layouts.Clear();

    this->isValid = false;
}

//------------------------------------------------------------------------------
/**
    Create a new layout object and start loading it. The loading usually
    is asynchronously (also see UIServerBase::Update() for details).
*/
Ptr<UiLayout>
UiServerBase::CreateLayout(const String& layoutId, const ResourceId& resId)
{
    if(this->layouts.FindIndex(layoutId) == InvalidIndex)
    {			
        Ptr<UiLayout> layout = UiLayout::Create();
		this->layouts.Add(layoutId, layout);
        layout->Setup(layoutId, resId);
        layout->Load();
        return layout;
    }
    else
    {
        return this->layouts[layoutId];    
    }
}

//------------------------------------------------------------------------------
/**
*/
const Ptr<UI::UiLayout>& 
UiServerBase::GetLayoutById( const Util::String& layoutId )
{
    n_assert(this->layouts.Contains(layoutId));
    return this->layouts[layoutId];
}

//------------------------------------------------------------------------------
/**
*/
void
UiServerBase::FreeLayout(const Util::String& layoutId)
{
	n_assert(this->layouts.Contains(layoutId));
	this->layouts[layoutId]->Discard();
	this->layouts.Erase(layoutId);
}
//------------------------------------------------------------------------------
/**
    Send event to all currently showing layouts
*/
bool 
UiServerBase::HandleInput( const Input::InputEvent& event )
{
    IndexT i;
    for (i = 0; i < this->layouts.Size(); i++)
    {
        const Ptr<UiLayout>& curLayout = this->layouts.ValueAtIndex(i);
        if (curLayout->IsShown())
        {        
            curLayout->HandleInput(event);
        }
    }
	return false;
}

//------------------------------------------------------------------------------
/**
    This method will be called exactly once per frame. Layouts should
    realize animation and other effects in this method. The method will
    also call the Load() method on all layouts which are still pending.
*/
void
UiServerBase::Update()
{
#ifdef _DEBUG
    if (!this->messagesPerFrame.IsEmpty())
    {
        n_printf("--------- Print Messages per Frame! ---------\n");
        Util::Array<String> messageKeys = this->messagesPerFrame.KeysAsArray();
        
        for (IndexT i = 0; i < messageKeys.Size(); i++)
        {
            n_printf("Message Id: %s\n", messageKeys[i].AsCharPtr());
            
            Util::Array<msgElement> values = this->messagesPerFrame[messageKeys[i]];
            while (values.Size() > 0)
            {
                n_printf("Layout: %s Element: %s\n", values[0].layoutId.Value(), values[0].elementId.Value());
                values.EraseIndex(0);
            }
            
            this->messagesPerFrame.Erase(messageKeys[i]);
        }
        
        this->messagesPerFrame.Clear();
    }
#endif
    
    // first update times, do a clean timer start in the first frame
    if (!this->timer.Running())
    {
        this->timer.Start();
    }
    this->curTime = this->timer.GetTime();
    this->curTicks = this->timer.GetTicks();

    IndexT i;
    for (i = 0; i < this->layouts.Size(); i++)
    {
        const Ptr<UiLayout>& curLayout = this->layouts.ValueAtIndex(i);
        if (curLayout->LoadFailed())
        {
            // loading the layout has failed
            n_error("UIServer: loading layout '%s' from res '%s' failed!\n", 
                curLayout->GetLayoutId().Value(),
                curLayout->GetResourceId().Value());
        }
        else if (curLayout->IsPending())
        {
            // layout still loading... continue to call Load()
            curLayout->Load();
        }
        else
        {
            curLayout->Update();
        }
    }
}

//------------------------------------------------------------------------------
/**
    This method will call the Render() method on all showable layouts. Note
    that Render() will even be called on layouts which are not in the 
    shown state, the layout object needs to decide itself whether it
    wants to render something!
*/
void
UiServerBase::Render(const Ptr<Frame::FrameBatch>& frameBatch)
{
    IndexT i;
    for (i = 0; i < this->layouts.Size(); i++)
    {
        const Ptr<UI::UiLayout>& layout = this->layouts.ValueAtIndex(i);
        if (layout->IsShown())
        {
            layout->Render(frameBatch);
        }
    }
}

//------------------------------------------------------------------------------
/**
*/
void 
UiServerBase::Resize(SizeT width, SizeT height)
{
	// empty
}   

//------------------------------------------------------------------------------
/**
*/
void
UiServerBase::LoadFont(const Util::String& resource, const Util::String& family, const Util::String& style, const Util::String& weight)
{
	n_error("not implemented");
}

//------------------------------------------------------------------------------
/**
*/
void
UiServerBase::LoadFont(const Util::String& resource)
{
	n_error("not implemented");
}


//------------------------------------------------------------------------------
/**
*/
Util::Array<Util::String>
UiServerBase::GetLayouts()
{
	return this->layouts.KeysAsArray();
}


} // namespace UI
