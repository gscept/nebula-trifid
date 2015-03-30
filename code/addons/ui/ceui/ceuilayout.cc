//------------------------------------------------------------------------------
//  CEUILayout.cc
//  (C) 2009 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "sui/ceui/ceuilayout.h"
#include "sui/ceui/ceuiserver.h"
#include "input/key.h"
#include "suiactions.h"
#include "sui/suievent.h"
#include "string.h"
#include "messaging/staticmessagehandler.h"
//#include "application/basegamefeature/managers/entitymanager.h"

using namespace Util;
using namespace Input;
using namespace Resources;
using namespace SUI;
using namespace CEUI;

namespace Messaging
{

	CEGUI::Window * WindowFromMessage(const Ptr<CEUILayout> &obj, const Ptr<SUIAction> &msg)
	{
		//Gets the element name, changes that to a StringAtom
		Util::StringAtom tempStringAtom = msg->GetElementId();
		//Convert the StringAtom to a regular nebula string;
		Util::String elemName = tempStringAtom.AsString();
		//The regular nebula string can now be converted to a Char Ptr and the element found
		return (CEGUI::Window*)(obj->GetLayout()->getChildRecursive(elemName.AsCharPtr()));
	}

__Handler(CEUILayout,SUIActionSetText)
{
	CEGUI::DefaultWindow* textComp = (CEGUI::DefaultWindow*)WindowFromMessage(obj,msg.downcast<SUIAction>());

	Util::String newString = msg->GetText();
	if(textComp != 0)
	{
		textComp->setText(newString.AsCharPtr());
	}
}

__Handler(CEUILayout,SUIActionAppendText)
{
	CEGUI::DefaultWindow* textComp = (CEGUI::DefaultWindow*)WindowFromMessage(obj,msg.downcast<SUIAction>());

	Util::String newString = msg->GetText();
	if(textComp != 0)
	{
		textComp->appendText(newString.AsCharPtr());		
	}
}

__Handler(CEUILayout,SUIActionGetText)
{
	CEGUI::DefaultWindow* textComp = (CEGUI::DefaultWindow*)WindowFromMessage(obj,msg.downcast<SUIAction>());

	if(textComp != 0)
	{
		msg->SetText(textComp->getText().c_str());
	}
}

__Handler(CEUILayout,SUIActionSetScrollSizes)
{

	CEGUI::Scrollbar* scrollComp = (CEGUI::Scrollbar*)WindowFromMessage(obj,msg.downcast<SUIAction>());

	float doc = msg->GetDocument();
	float page = msg->GetPage();
	float step = msg->GetStep();
	if(scrollComp != 0)
	{
		scrollComp->setDocumentSize(doc);
		scrollComp->setPageSize(page);
		scrollComp->setStepSize(step);
	}
}

__Handler(CEUILayout,SUIActionSetScrollPosition)
{
	CEGUI::Scrollbar* scrollComp = (CEGUI::Scrollbar*)WindowFromMessage(obj,msg.downcast<SUIAction>());
	if(scrollComp != 0)
	{
		scrollComp->setScrollPosition(msg->Getposition());
	}
}

__Handler(CEUILayout,SUIActionSetVisible)
{
	//FIXME UNTESTED
	CEGUI::Window* win = (CEGUI::Window*)WindowFromMessage(obj,msg.downcast<SUIAction>());
	if(win != 0)
	{
		win->setVisible(msg->GetVisible());
	}
}

__Handler(CEUILayout,SUIActionSetSize)
{
	//FIXME UNTESTED
	CEGUI::Window* win = (CEGUI::Window*)WindowFromMessage(obj,msg.downcast<SUIAction>());
	if(win != 0)
	{
		Math::float2 s = msg->GetSize();
		win->setSize(CEGUI::UVector2(cegui_absdim(s.x()),cegui_absdim(s.y())));
	}
}

__Handler(CEUILayout,SUIActionGetPosition)
{
	//FIXME UNTESTED
	CEGUI::Window* win = (CEGUI::Window*)WindowFromMessage(obj,msg.downcast<SUIAction>());
	if(win != 0)
	{
		// size probably broken, i guess it wants the window size? no idea...
		CEGUI::Vector2 pos = win->getPosition().asAbsolute(CEGUI::Size(1,1));
		msg->SetPosition(Math::float2(pos.d_x,pos.d_y));		
	}
}

__Handler(CEUILayout,SUIActionSetStepSize)
{
	//FIXME UNTESTED
	CEGUI::Scrollbar* scrollComp = (CEGUI::Scrollbar*)WindowFromMessage(obj,msg.downcast<SUIAction>());
	if(scrollComp != 0)
	{
		scrollComp->setStepSize(msg->GetStepSize());
	}
}

__Handler(CEUILayout,SUIActionAddListItem)
{
	//FIXME UNTESTED
	CEGUI::Listbox * listbox = (CEGUI::Listbox *)WindowFromMessage(obj,msg.downcast<SUIAction>());
	if(listbox != 0)
	{
		CEGUI::ListboxTextItem * newItem = new CEGUI::ListboxTextItem(msg->GetText().AsCharPtr(),0,msg->GetUserData());
		listbox->addItem(newItem);
	}
}

__Handler(CEUILayout,SUIActionSetImage)
{
	CEGUI::DefaultWindow* imageComp = (CEGUI::DefaultWindow*)WindowFromMessage(obj,msg.downcast<SUIAction>());

	Util::String nebulaString = msg->GetImageName();
	std::string stdString = nebulaString.AsCharPtr();
	CEGUI::String ceguiString(nebulaString.AsCharPtr());
	if(imageComp != 0)
	{
		imageComp->setProperty("Image",ceguiString);// "set:Fullimage_imgSet image:img");
	}
}

__Handler(CEUILayout,SUIActionSetProgress)
{
	CEGUI::ProgressBar* progressComp = (CEGUI::ProgressBar*)WindowFromMessage(obj,msg.downcast<SUIAction>());
	progressComp->setProgress(msg->GetProgress());
}

__Handler(CEUILayout,SUIActionSetEnabled)
{
	CEGUI::PushButton* buttonComp = (CEGUI::PushButton*)WindowFromMessage(obj,msg.downcast<SUIAction>());
	buttonComp->setEnabled(msg->GetEnabled());
}

__Handler(CEUILayout,SUIActionSetSelected)
{
	CEGUI::Checkbox* checkboxComp = (CEGUI::Checkbox*)WindowFromMessage(obj,msg.downcast<SUIAction>());
	checkboxComp->setSelected(msg->GetSelected());
}

__Handler(CEUILayout,SUIActionAddComboboxItem)
{
	CEGUI::Combobox* comboboxComp = (CEGUI::Combobox*)WindowFromMessage(obj,msg.downcast<SUIAction>());
	Util::String nebulaString = msg->GetText();
	CEGUI::String ceguiString(nebulaString.AsCharPtr());
	
	CEGUI::ListboxTextItem* itemCombobox = new CEGUI::ListboxTextItem(ceguiString);
	comboboxComp->addItem(itemCombobox);
	itemCombobox->setSelected(true);
	comboboxComp->setText(itemCombobox->getText());
	
}

__Handler(CEUILayout,SUIActionGetComboboxItem)
{

	CEGUI::Combobox* comboboxComp = (CEGUI::Combobox*)WindowFromMessage(obj,msg.downcast<SUIAction>());
	CEGUI::Editbox* editbox = comboboxComp->getEditbox();
	editbox->setReadOnly(true);
	CEGUI::String outtext = editbox->getText();
	msg->SetText(outtext.c_str());
}

__Handler(CEUILayout,SUIActionEmptyListBox)
{
	CEGUI::Listbox* listbox = (CEGUI::Listbox*)WindowFromMessage(obj, msg.downcast<SUIAction>());
	listbox->resetList();
	/*listbox->clearAllSelections();*/
	
	/*int numItems = listbox->getItemCount();

	for (int i(0); i < numItems ; i++)
	{
		CEGUI::ListboxItem* tempItem = listbox->getListboxItemFromIndex(i-1);
		listbox->removeItem(tempItem);
	}*/
}


__Dispatcher(CEUILayout)
{
	__Handle(CEUILayout,SUIActionSetVisible);
	__Handle(CEUILayout,SUIActionSetSize);
//	__Handle(CEUILayout,SUIActionPlayLayoutAnim);
//	__Handle(CEUILayout,SUIActionPlayElementAnim);
//	__Handle(CEUILayout,SUIActionHasAnimFinished);
//	__Handle(CEUILayout,SUIActionSetPaused);
	__Handle(CEUILayout,SUIActionSetText);
	__Handle(CEUILayout,SUIActionGetText);
	__Handle(CEUILayout,SUIActionAppendText);
	// this is not so easy
//	__Handle(CEUILayout,SUIActionSetFontSize);
	__Handle(CEUILayout,SUIActionGetPosition);

	__Handle(CEUILayout,SUIActionSetStepSize);
	__Handle(CEUILayout,SUIActionAddListItem);
	__Handle(CEUILayout,SUIActionSetScrollSizes);
	__Handle(CEUILayout,SUIActionSetScrollPosition);	
	__Handle(CEUILayout,SUIActionSetImage);
	__Handle(CEUILayout,SUIActionSetProgress);
	__Handle(CEUILayout,SUIActionSetEnabled);
	__Handle(CEUILayout,SUIActionSetSelected);
	__Handle(CEUILayout,SUIActionAddComboboxItem);
	__Handle(CEUILayout,SUIActionGetComboboxItem);
	__Handle(CEUILayout,SUIActionEmptyListBox);
}

}


namespace CEUI
{
__ImplementClass(CEUI::CEUILayout, 'CULY', Base::SUILayoutBase);


CEGUI::MouseButton 
MouseButtonToCEGUI(MouseButton::Code b)
{
	switch(b)
	{
	case MouseButton::LeftButton:
		return CEGUI::LeftButton;
		break;
	case MouseButton::RightButton:
		return CEGUI::RightButton;
		break;
	case MouseButton::MiddleButton:
		return  CEGUI::RightButton;
		break;
	default:
		return CEGUI::LeftButton;
	}
}

//------------------------------------------------------------------------------
/**
*/
CEUILayout::CEUILayout() :
    ceguiLayout(0)
{
}

//------------------------------------------------------------------------------
/**
*/
CEUILayout::~CEUILayout()
{
    n_assert(!this->IsValid());
}
//------------------------------------------------------------------------------
/**
*/
void
CEUILayout::Discard()
{
    n_assert(0 != this->ceguiLayout);
    n_assert(0 != CEUIServer::Instance()->wndManager);

    // do special CEUI discard stuff here
    CEUIServer::Instance()->rootSheet->removeChildWindow(this->ceguiLayout->getID());
    CEUIServer::Instance()->wndManager->destroyWindow(this->ceguiLayout);
    this->ceguiLayout = 0;
    this->resState = Resource::Initial;
    
    // call discard of parent class
    Base::SUILayoutBase::Discard();
}

//------------------------------------------------------------------------------
/**
    Start or continue loading the layout resource. Loading should happen
    asynchronously. As long as the IsPending() method returns true, the
    app should continue to call Load() until IsPending() returns false.
    If loading has failed, the LoadFailed() method will return true.
*/
void
CEUILayout::Load()
{
    n_assert(0 == this->ceguiLayout);
    n_assert(0 != CEUIServer::Instance()->cegui);
    n_assert(0 != CEUIServer::Instance()->wndManager);
    n_assert(this->IsValid());
    
    // Load CEGUI layout from file
	CEUIServer* foo = CEUIServer::Instance();
	CEGUI::WindowManager* faa = foo->wndManager;
    this->ceguiLayout = CEUIServer::Instance()->wndManager->loadWindowLayout(this->resourceId.Value());
    CEUIServer::Instance()->rootSheet->addChildWindow(this->ceguiLayout);
    this->resState = Resource::Loaded;

	this->RegisterHandlers();
	batchEvents = new Array<SUI::SUIEvent>();
}

//------------------------------------------------------------------------------
/**
    Call this method to make the layout visible.
*/
void
CEUILayout::Show()
{
    n_assert(0 != this->ceguiLayout);
    
    // show ceui layout here
    if (!this->shown)
    {
        this->ceguiLayout->show();		
    }
    
    // call show of parent class
    Base::SUILayoutBase::Show();
}

//------------------------------------------------------------------------------
/**
    Call this method to make the layout invisible.
*/
void
CEUILayout::Hide()
{    
    n_assert(0 != this->ceguiLayout);
    
    // hide ceui layout here
    if (this->shown)
    {
        this->ceguiLayout->hide();
    }

	//call hide of parent class
	Base::SUILayoutBase::Hide();
	CEUIServer::Instance()->cegui->signalRedraw();
}
//------------------------------------------------------------------------------
/**
    Update the layout
*/
void CEUILayout::Update()
{
	//BaseGameFeature::EntityManager::Instance()->Send(Message::Create());
}
//------------------------------------------------------------------------------
/**
    Register the layout handler.
*/
void
CEUILayout::RegisterHandlers()
{
	
}

//------------------------------------------------------------------------------
/**
    This method takes an array of input events, and produce a resulting 
    SUIEvents which must be appended(!) to the outEvents array. The method
    must return the number of produced SUIEvents.
*/
SizeT
CEUILayout::ProcessInputEvents(const Array<InputEvent>& inputEvents, Array<SUI::SUIEvent>& outEvents)
{
	int newEvents = 0;
	for(int i = 0 ; i < inputEvents.Size();i++)
	{
		InputEvent ev = inputEvents[i];
		batchEvents->Clear();
		bool handled = false;
		switch(ev.GetType())
		{
			case InputEvent::MouseMove:
				{
					Math::float2 pos = ev.GetAbsMousePos();					
					//handled = CEUIServer::Instance()->cegui->injectMousePosition(pos.x(),pos.y());
					Math::float2 npos = ev.GetNormMousePos();
					CEGUI::Rect rec = this->ceguiLayout->getOuterRectClipper();
					float x = rec.getWidth() * npos.x();
					float y = rec.getHeight() * npos.y();
					handled = CEUIServer::Instance()->cegui->injectMousePosition(x,y);					
				}				
				break;
			case InputEvent::MouseButtonDown:
				handled = CEUIServer::Instance()->cegui->injectMouseButtonDown(MouseButtonToCEGUI(ev.GetMouseButton()));
				break;
			case InputEvent::MouseButtonUp:
				handled = CEUIServer::Instance()->cegui->injectMouseButtonUp(MouseButtonToCEGUI(ev.GetMouseButton()));
				break;
			case InputEvent::KeyDown:
				handled = CEUIServer::Instance()->cegui->injectKeyDown(Input::Key::ToDirectInput(ev.GetKey()));
				break;
			case InputEvent::KeyUp:
				handled = CEUIServer::Instance()->cegui->injectKeyUp(Input::Key::ToDirectInput(ev.GetKey()));			
				break;
			case InputEvent::Character:
				handled = CEUIServer::Instance()->cegui->injectChar(ev.GetChar());
				break;
			default:
				break;
		}
		if(handled)
		{
			for(Util::Array<SUI::SUIEvent>::Iterator iter = batchEvents->Begin(); iter != batchEvents->End();iter++)
			{
				iter->SetInputEvent(ev);
				outEvents.Append(*iter);
				newEvents++;
			}
		}
	}
    return newEvents;
}

bool
CEUILayout::HandleEvent(const CEGUI::KeyEventArgs& ev)
{
	SUIEvent keyev;
	if(ev.window!=NULL)
	{
		keyev.Setup(this->GetLayoutId(),ev.window->getName().c_str(),SUIEvent::KeyEvent);
		keyev.SetObject(ev.window);
		this->batchEvents->Append(keyev);		
	}
	return false;
}
bool
CEUILayout::HandleEvent(const CEGUI::WindowEventArgs& ev)
{	
	SUIEvent keyev;
	if(ev.window!=NULL)
	{
		keyev.Setup(this->GetLayoutId(),ev.window->getName().c_str(),SUIEvent::WindowEvent);
		keyev.SetObject(ev.window);
		this->batchEvents->Append(keyev);		
	}
	return false;
}
bool
CEUILayout::HandleEvent(const CEGUI::MouseEventArgs& ev)
{
	SUIEvent keyev;
	if(ev.window!=NULL)
	{
		keyev.Setup(this->GetLayoutId(),ev.window->getName().c_str(),SUIEvent::MouseEvent);
		keyev.SetObject(ev.window);
		this->batchEvents->Append(keyev);		
	}
	return false;
}

bool
CEUILayout::HandleMouseEnterEvent(const CEGUI::MouseEventArgs& ev)
{
	SUIEvent keyev;
	if(ev.window!=NULL)
	{		
		keyev.Setup(this->GetLayoutId(),ev.window->getName().c_str(),SUIEvent::MouseEnter);
		keyev.SetObject(ev.window);
		this->batchEvents->Append(keyev);		
	}
	return false;
}

bool
CEUILayout::HandleMouseLeaveEvent(const CEGUI::MouseEventArgs& ev)
{
	SUIEvent keyev;
	if(ev.window!=NULL)
	{
		keyev.Setup(this->GetLayoutId(),ev.window->getName().c_str(),SUIEvent::MouseLeave);
		keyev.SetObject(ev.window);
		this->batchEvents->Append(keyev);		
	}
	return false;
}


//------------------------------------------------------------------------------
/**
    Invoke an action on the layout. This is the only way to manipulate the
    contents of the from the outside.
*/
void
CEUILayout::InvokeAction(const Ptr<SUI::SUIAction>& action)
{    
	__Dispatch(CEUILayout,this,action.downcast<Messaging::Message>());	
}

} // namespace CEUI