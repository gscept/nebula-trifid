//------------------------------------------------------------------------------
//  qt/qtfeatureunit.cc
//  (C) 2012-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include <QEventLoop>
#include "qtfeature/qtfeatureunit.h"
#include "qapplication.h"
#include "input/inputserver.h"
#include "qtinputproxy.h"
#include "input/keyboard.h"


using namespace Input;
namespace QtFeature
{
__ImplementClass(QtFeatureUnit, 'QTFU' , Game::FeatureUnit);
__ImplementSingleton(QtFeatureUnit);



//------------------------------------------------------------------------------
/**
*/
QtFeatureUnit::QtFeatureUnit()
{	
    __ConstructSingleton;
}

//------------------------------------------------------------------------------
/**
*/
QtFeatureUnit::~QtFeatureUnit()
{
    __DestructSingleton;
}

//------------------------------------------------------------------------------
/**
*/
void
QtFeatureUnit::OnActivate()
{
    n_assert(!this->qtServer.isvalid());
    this->qtServer = QtServer::Create();
    this->qtServer->Open();
    FeatureUnit::OnActivate();
}

//------------------------------------------------------------------------------
/**
*/
void
QtFeatureUnit::OnDeactivate()
{
    n_assert(this->qtServer.isvalid());
    this->qtServer->Close();
    this->qtServer = 0;
    FeatureUnit::OnDeactivate();    
}

//------------------------------------------------------------------------------
/**
*/
void
QtFeatureUnit::OnRenderDebug()
{
    
}

//------------------------------------------------------------------------------
/**
*/
void
QtFeatureUnit::OnBeginFrame()
{    
    this->qtServer->Trigger();
	Qt::KeyboardModifiers mods = QApplication::queryKeyboardModifiers();
	const Ptr<Input::Keyboard>& kbd = Input::InputServer::Instance()->GetDefaultKeyboard();

	if (mods & Qt::ShiftModifier)		kbd->SetKeyDown(Key::Shift);
	else								kbd->SetKeyUp(Key::Shift);

	if (mods & Qt::ControlModifier)		kbd->SetKeyDown(Key::Control);
	else								kbd->SetKeyUp(Key::Control);

	if (mods & Qt::AltModifier)			kbd->SetKeyDown(Key::Menu);
	else								kbd->SetKeyUp(Key::Menu);

    FeatureUnit::OnFrame();
}

//------------------------------------------------------------------------------
/**
*/
void
QtFeatureUnit::RegisterQtInputProxy(QObject * r)
{    
    this->qtinputProxy = QtInputProxy::Create();
    this->qtinputProxy->SetReceiver(r);
    Input::InputServer::Instance()->AttachInputHandler(Input::InputPriority::Gui, this->qtinputProxy.cast<Input::InputHandler>());
}

//------------------------------------------------------------------------------
/**
*/
void
QtFeatureUnit::DeregisterQtInputProxy()
{    
    Input::InputServer::Instance()->RemoveInputHandler(this->qtinputProxy.cast<Input::InputHandler>());
    this->qtinputProxy = 0;
}

//------------------------------------------------------------------------------
/**
*/
void
QtFeatureUnit::SendDownKeyEvent(QKeyEvent* event)
{
	Qt::Key key = (Qt::Key)event->key();
	Key::Code code = (Key::Code)QtInputProxy::Qt2NebInputKey(key);
	InputEvent inputEvent;
	inputEvent.SetType(InputEvent::KeyDown);
	inputEvent.SetKey(code);
	
	this->qtinputProxy->SetIgnoreEvents(true);
	InputServer::Instance()->PutEvent(inputEvent);
	this->qtinputProxy->SetIgnoreEvents(false);
}

//------------------------------------------------------------------------------
/**
*/
void
QtFeatureUnit::SendUpKeyEvent(QKeyEvent* event)
{
	Qt::Key key = (Qt::Key)event->key();
	Key::Code code = (Key::Code)QtInputProxy::Qt2NebInputKey(key);
	InputEvent inputEvent;
	inputEvent.SetType(InputEvent::KeyUp);
	inputEvent.SetKey(code);

	this->qtinputProxy->SetIgnoreEvents(true);
	InputServer::Instance()->PutEvent(inputEvent);
	this->qtinputProxy->SetIgnoreEvents(false);
}

}; // namespace QTFeature
