//------------------------------------------------------------------------------
//  suiserver.cc
//  (C) 2009 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "sui/ceui/ceuiserver.h"
#include "io/assignregistry.h"
#include "io/ioserver.h"

namespace CEUI
{
	using namespace Util;
	using namespace Resources;
	using namespace SUI;
	using namespace Input;
	using namespace CEGUI;

__ImplementClass(CEUI::CEUIServer, 'CESV', Base::SUIServerBase);
__ImplementSingleton(CEUI::CEUIServer);

//------------------------------------------------------------------------------
/**
*/
CEUIServer::CEUIServer() :
    renderer(0),
    resourceProvider(0),
    cegui(0),
    wndManager(0),
    rootSheet(0),
    shdInst(0)
{
    __ConstructSingleton;
}

//------------------------------------------------------------------------------
/**
*/
CEUIServer::~CEUIServer()
{
    __DestructSingleton;
}

//------------------------------------------------------------------------------
/**
*/
void
CEUIServer::Setup()
{
    // call parent
    SUIServerBase::Setup();

    // setup CEGUI system
    this->renderer = CEUI::CEUIRenderer::Create();
    this->resourceProvider = n_new(CEUI::CEUIResourceProvider);
    this->cegui = &CEGUI::System::create(*this->renderer, this->resourceProvider);
    n_assert(0 != this->cegui);

    // setup resource paths
    IO::AssignRegistry::Instance()->SetAssign(IO::Assign("ceui", "export:/ceui/datafiles/"));

    CEUI::CEUIResourceProvider* resProvider = 
        static_cast<CEUI::CEUIResourceProvider*>(this->cegui->getResourceProvider());
    resProvider->setResourceGroupDirectory("schemes", "ceui:schemes/");
    resProvider->setResourceGroupDirectory("imagesets", "ceui:imagesets/");
    resProvider->setResourceGroupDirectory("fonts", "ceui:fonts/");
    resProvider->setResourceGroupDirectory("layouts", "ceui:layouts/");
    resProvider->setResourceGroupDirectory("looknfeel", "ceui:looknfeel/");
    resProvider->setResourceGroupDirectory("lua_scripts", "ceui:lua_scripts/");

    CEGUI::Imageset::setDefaultResourceGroup("imagesets");
    CEGUI::Font::setDefaultResourceGroup("fonts");
    CEGUI::Scheme::setDefaultResourceGroup("schemes");
    CEGUI::WidgetLookManager::setDefaultResourceGroup("looknfeel");
    CEGUI::WindowManager::setDefaultResourceGroup("layouts");
    CEGUI::ScriptModule::setDefaultResourceGroup("lua_scripts");

    // create (load) the TaharezLook scheme file
    // (this auto-loads the TaharezLook looknfeel and imageset files)

	Util::Array<Util::String> schemeFiles = IO::IoServer::Instance()->ListFiles("ceui:schemes/", "*.scheme");
	for(IndexT index = 0; index < schemeFiles.Size(); index++)
	{
		CEGUI::String scheme = schemeFiles[index].ExtractFileName().AsCharPtr();
		CEGUI::SchemeManager::getSingleton().create(scheme);
	}
//     CEGUI::SchemeManager::getSingleton().create( "TaharezLook.scheme" );
// 	CEGUI::SchemeManager::getSingleton().create( "OgreTray.scheme" );
// 	CEGUI::SchemeManager::getSingleton().create( "VanillaSkin.scheme" );
// 	CEGUI::SchemeManager::getSingleton().create( "WindowsLook.scheme" );
// 	CEGUI::SchemeManager::getSingleton().create( "Boliden.scheme" );

    // create (load) a font.
    // The first font loaded automatically becomes the default font, but note
    // that the scheme might have already loaded a font, so there may already
    // be a default set - if we want the "Commonweath-10" font to definitely
    // be the default, we should set the default explicitly afterwards.
	CEGUI::Font& font = CEGUI::FontManager::getSingleton().create( "DejaVuSans-10.font" );

    // create the root sheet as a aprent for all other windows
    this->wndManager = CEGUI::WindowManager::getSingletonPtr();
    n_assert(0 != this->wndManager);
    this->rootSheet = this->wndManager->createWindow("DefaultWindow", "Nebula3RootSheet");
    n_assert(0 != this->rootSheet);
    this->cegui->setGUISheet(this->rootSheet);

	// register all events
	CEGUI::GlobalEventSet::getSingleton().subscribeEvent(Window::EventNamespace + "/" + Window::EventKeyDown,CEGUI::SubscriberSlot(&CEUIServer::HandleKeyEvent, this));
	CEGUI::GlobalEventSet::getSingleton().subscribeEvent(Window::EventNamespace + "/" + Window::EventKeyUp,CEGUI::SubscriberSlot(&CEUIServer::HandleKeyEvent, this));
	CEGUI::GlobalEventSet::getSingleton().subscribeEvent(Window::EventNamespace + "/" + Window::EventMouseButtonDown,CEGUI::SubscriberSlot(&CEUIServer::HandleMouseEvent, this));
	CEGUI::GlobalEventSet::getSingleton().subscribeEvent(Window::EventNamespace + "/" + Window::EventMouseButtonUp,CEGUI::SubscriberSlot(&CEUIServer::HandleMouseEvent, this));
	CEGUI::GlobalEventSet::getSingleton().subscribeEvent(Window::EventNamespace + "/" + Window::EventMouseClick,CEGUI::SubscriberSlot(&CEUIServer::HandleMouseEvent, this));
	CEGUI::GlobalEventSet::getSingleton().subscribeEvent(Window::EventNamespace + "/" + Window::EventMouseDoubleClick,CEGUI::SubscriberSlot(&CEUIServer::HandleMouseEvent, this));
	CEGUI::GlobalEventSet::getSingleton().subscribeEvent(Window::EventNamespace + "/" + Window::EventMouseEnters,CEGUI::SubscriberSlot(&CEUIServer::HandleMouseEnterEvent, this));
	CEGUI::GlobalEventSet::getSingleton().subscribeEvent(Window::EventNamespace + "/" + Window::EventMouseLeaves,CEGUI::SubscriberSlot(&CEUIServer::HandleMouseLeaveEvent, this));
	CEGUI::GlobalEventSet::getSingleton().subscribeEvent(Window::EventNamespace + "/" + Window::EventMouseWheel,CEGUI::SubscriberSlot(&CEUIServer::HandleMouseEvent, this));	
	CEGUI::GlobalEventSet::getSingleton().subscribeEvent(Checkbox::EventNamespace + "/" + Checkbox::EventCheckStateChanged,CEGUI::SubscriberSlot(&CEUIServer::HandleWindowEvent, this));
	CEGUI::GlobalEventSet::getSingleton().subscribeEvent(Editbox::EventNamespace + "/" + Editbox::EventTextAccepted,CEGUI::SubscriberSlot(&CEUIServer::HandleWindowEvent, this));
	CEGUI::GlobalEventSet::getSingleton().subscribeEvent(MenuItem::EventNamespace + "/" + MenuItem::EventClicked,CEGUI::SubscriberSlot(&CEUIServer::HandleWindowEvent, this));
	CEGUI::GlobalEventSet::getSingleton().subscribeEvent(PushButton::EventNamespace + "/" + PushButton::EventClicked,CEGUI::SubscriberSlot(&CEUIServer::HandleWindowEvent, this));
	CEGUI::GlobalEventSet::getSingleton().subscribeEvent(Scrollbar::EventNamespace + "/" + Scrollbar::EventScrollPositionChanged,CEGUI::SubscriberSlot(&CEUIServer::HandleWindowEvent, this));	
	CEGUI::GlobalEventSet::getSingleton().subscribeEvent(Slider::EventNamespace + "/" + Slider::EventValueChanged,CEGUI::SubscriberSlot(&CEUIServer::HandleWindowEvent, this));
	CEGUI::GlobalEventSet::getSingleton().subscribeEvent(Spinner::EventNamespace + "/" + Spinner::EventValueChanged,CEGUI::SubscriberSlot(&CEUIServer::HandleWindowEvent, this));	

}

//------------------------------------------------------------------------------
/**
*/
void
CEUIServer::Discard()
{
    // call parent
    SUIServerBase::Discard();

    // release shader instance
    this->shdInst = 0;

    // remove root sheet
    CEGUI::System::getSingletonPtr()->setGUISheet(NULL);
    CEGUI::WindowManager::getSingletonPtr()->destroyAllWindows();
    this->rootSheet = 0;
    this->wndManager = 0;

    // discard CEGUI system
    CEGUI::System::destroy();
    this->cegui = 0;

    // delete resouce provider
    n_delete(this->resourceProvider);
    this->resourceProvider = 0;

    // destroy renderer
    this->renderer = 0;
}

//------------------------------------------------------------------------------
/**
    Update the SUI system, called exactly once per frame
*/
void
CEUIServer::Update()
{
	((Base::SUIServerBase*)this)->Update();
}

//------------------------------------------------------------------------------
/**
    Render SUI system, may be called several times per frame
*/
void
CEUIServer::Render(const Ptr<Frame::FrameBatch>& frameBatch)
{
    n_assert(0 != this->cegui);
    n_assert(this->renderer.isvalid());

    if(CEUI::CEUIRenderer::Instance()->CheckAllTextureReady())
    {
        this->renderer->SetShaderInstance(frameBatch->GetShader());
        cegui->renderGUI();
    }
}

//------------------------------------------------------------------------------
/**
    Render SUI system, may be called several times per frame
*/
void CEUIServer::Render( const Ptr<CoreGraphics::ShaderInstance>& shader )
{
	n_assert(0 != this->cegui);
	n_assert(this->renderer.isvalid());

	if(CEUI::CEUIRenderer::Instance()->CheckAllTextureReady())
	{
		this->renderer->SetShaderInstance(shader);
		cegui->renderGUI();
	}
}

Util::Array<SUI::SUIEvent>
CEUIServer::ProcessInputEvents(const Array<InputEvent>& inputEvents)
{
	Util::Array<SUI::SUIEvent> result;
	IndexT i;
	for (i = 0; i < this->layouts.Size(); i++)
	{		
		const Ptr<SUILayout>& curLayout = this->layouts.ValueAtIndex(i);
		currentInputTarget = curLayout;
		if (curLayout->IsShown())
		{        
			curLayout->ProcessInputEvents(inputEvents, result);
		}
		currentInputTarget = 0;
	}
	return result;
}

bool 
CEUIServer::HandleKeyEvent(const CEGUI::EventArgs&ev)
{
	if(currentInputTarget.isvalid())
	{
		return currentInputTarget->HandleEvent((const CEGUI::KeyEventArgs &)ev);
	}
	return false;
}

bool 
CEUIServer::HandleWindowEvent(const CEGUI::EventArgs&ev)
{
	if(currentInputTarget.isvalid())
	{
		return currentInputTarget->HandleEvent((const CEGUI::WindowEventArgs &)ev);
	}
	return false;
}

bool 
CEUIServer::HandleMouseEvent(const CEGUI::EventArgs&ev)
{
	if(currentInputTarget.isvalid())
	{
		return currentInputTarget->HandleEvent((const CEGUI::MouseEventArgs &)ev);
	}
	return false;
}

bool 
CEUIServer::HandleMouseLeaveEvent(const CEGUI::EventArgs&ev)
{
	if(currentInputTarget.isvalid())
	{
		return currentInputTarget->HandleMouseLeaveEvent((const CEGUI::MouseEventArgs &)ev);
	}
	return false;
}bool 
CEUIServer::HandleMouseEnterEvent(const CEGUI::EventArgs&ev)
{
	if(currentInputTarget.isvalid())
	{
		return currentInputTarget->HandleMouseEnterEvent((const CEGUI::MouseEventArgs &)ev);
	}
	return false;
}
} // namespace CEUI