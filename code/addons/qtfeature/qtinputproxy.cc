//------------------------------------------------------------------------------
//  qtinputproxy.cc
//  (C) 2013-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "qtfeature/qtinputproxy.h"
#include "input/inputserver.h"
#include "input/keyboard.h"
#include "input/key.h"
#include <QCoreApplication>
#include <QApplication>
#include <QKeyEvent>
#include <QWidget>

namespace QtFeature
{
__ImplementClass(QtFeature::QtInputProxy, 'QINP', Input::InputHandler);
using namespace Core;
using namespace Util;
using namespace Input;
//------------------------------------------------------------------------------
/**
*/
QtInputProxy::QtInputProxy() : 
	receiver(NULL),
	ignore(false)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
QtInputProxy::~QtInputProxy()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
int
QtInputProxy::Neb2QtInputKey(const Key::Code & inCode)
{    
    switch(inCode)
    {
        case Key::Space:         return Qt::Key_Space;
        case Key::Key0:          return Qt::Key_0;
        case Key::Key1:          return Qt::Key_1;
        case Key::Key2:          return Qt::Key_2;
        case Key::Key3:          return Qt::Key_3;
        case Key::Key4:          return Qt::Key_4;
        case Key::Key5:          return Qt::Key_5;
        case Key::Key6:          return Qt::Key_6;
        case Key::Key7:          return Qt::Key_7;
        case Key::Key8:          return Qt::Key_8;
        case Key::Key9:          return Qt::Key_9;
        case Key::A:             return Qt::Key_A;
        case Key::B:             return Qt::Key_B;
        case Key::C:             return Qt::Key_C;
        case Key::D:             return Qt::Key_D;
        case Key::E:             return Qt::Key_E;
        case Key::F:             return Qt::Key_F;
        case Key::G:             return Qt::Key_G;
        case Key::H:             return Qt::Key_H;
        case Key::I:             return Qt::Key_I;
        case Key::J:             return Qt::Key_J;
        case Key::K:             return Qt::Key_K;
        case Key::L:             return Qt::Key_L;
        case Key::M:             return Qt::Key_M;
        case Key::N:             return Qt::Key_N;
        case Key::O:             return Qt::Key_O;
        case Key::P:             return Qt::Key_P;
        case Key::Q:             return Qt::Key_Q;
        case Key::R:             return Qt::Key_R;
        case Key::S:             return Qt::Key_S;
        case Key::T:             return Qt::Key_T;
        case Key::U:             return Qt::Key_U;
        case Key::V:             return Qt::Key_V;
        case Key::W:             return Qt::Key_W;
        case Key::X:             return Qt::Key_X;
        case Key::Y:             return Qt::Key_Y;
        case Key::Z:             return Qt::Key_Z;   
        case Key::Semicolon:     return Qt::Key_Semicolon;
        case Key::Add:           return Qt::Key_Plus;
        case Key::Comma:         return Qt::Key_Comma;
        case Key::Subtract:      return Qt::Key_Minus;
        case Key::Period:        return Qt::Key_Period;
        case Key::Slash:         return Qt::Key_Slash;
        case Key::Tilde:         return Qt::Key_AsciiTilde;
        case Key::LeftBracket:   return Qt::Key_BracketLeft;
        case Key::BackSlash:     return Qt::Key_Backslash;
        case Key::RightBracket:  return Qt::Key_BracketRight;
        case Key::Quote:         return Qt::Key_QuoteDbl;                

        case Key::NumPad0:       return Qt::Key_0;
        case Key::NumPad1:       return Qt::Key_1;
        case Key::NumPad2:       return Qt::Key_2;
        case Key::NumPad3:       return Qt::Key_3;
        case Key::NumPad4:       return Qt::Key_4;
        case Key::NumPad5:       return Qt::Key_5;
        case Key::NumPad6:       return Qt::Key_6;
        case Key::NumPad7:       return Qt::Key_7;
        case Key::NumPad8:       return Qt::Key_8;
        case Key::NumPad9:       return Qt::Key_9;

        case Key::Back:          return Qt::Key_Back;
        case Key::Tab:           return Qt::Key_Tab;
        case Key::Clear:         return Qt::Key_Clear;
        case Key::Return:        return Qt::Key_Return;
        case Key::Pause:         return Qt::Key_Pause;        

        case Key::Escape:        return Qt::Key_Escape;
                        
        case Key::End:           return Qt::Key_End;
        case Key::Home:          return Qt::Key_Home;
        case Key::Left:          return Qt::Key_Left;
        case Key::Up:            return Qt::Key_Up;
        case Key::Right:         return Qt::Key_Right;
        case Key::Down:          return Qt::Key_Down;                
                
        case Key::Insert:        return Qt::Key_Insert;
        case Key::Delete:        return Qt::Key_Delete;
        
        case Key::RightWindows:
        case Key::LeftWindows:   return Qt::Key_Meta;              
        
        
        case Key::F1:                        return Qt::Key_F1;
        case Key::F2:                        return Qt::Key_F2;
        case Key::F3:                        return Qt::Key_F3;
        case Key::F4:                        return Qt::Key_F4;
        case Key::F5:                        return Qt::Key_F5;
        case Key::F6:                        return Qt::Key_F6;
        case Key::F7:                        return Qt::Key_F7;
        case Key::F8:                        return Qt::Key_F8;
        case Key::F9:                        return Qt::Key_F9;
        case Key::F10:                       return Qt::Key_F10;
        case Key::F11:                       return Qt::Key_F11;
        case Key::F12:                       return Qt::Key_F12;
        case Key::F13:                       return Qt::Key_F13;
        case Key::F14:                       return Qt::Key_F14;
        case Key::F15:                       return Qt::Key_F15;
        case Key::F16:                       return Qt::Key_F16;
        case Key::F17:                       return Qt::Key_F17;
        case Key::F18:                       return Qt::Key_F18;
        case Key::F19:                       return Qt::Key_F19;
        case Key::F20:                       return Qt::Key_F20;
        case Key::F21:                       return Qt::Key_F21;
        case Key::F22:                       return Qt::Key_F22;
        case Key::F23:                       return Qt::Key_F23;
        case Key::F24:                       return Qt::Key_F24;

        case Key::NumLock:           return Qt::Key_NumLock;
        case Key::Scroll:            return Qt::Key_ScrollLock;

        case Key::LeftShift:         return Qt::Key_Shift;
        case Key::RightShift:        return Qt::Key_Shift;
        case Key::LeftControl:       return Qt::Key_Control;
        case Key::RightControl:      return Qt::Key_Control;
        case Key::LeftMenu:          return Qt::Key_Meta;
        case Key::RightMenu:         return Qt::Key_Meta;
        
        default:
                break;
        }
        //n_error("Invalid key code!");
        return 0;
}

//------------------------------------------------------------------------------
/**
*/
int
QtInputProxy::Qt2NebInputKey(const Qt::Key& inCode)
{
	switch (inCode)
	{
	case Qt::Key_Space:			return Key::Space;
	case Qt::Key_0:				return Key::Key0;
	case Qt::Key_1:				return Key::Key1;
	case Qt::Key_2:				return Key::Key2;
	case Qt::Key_3:				return Key::Key3;
	case Qt::Key_4:				return Key::Key4;
	case Qt::Key_5:				return Key::Key5;
	case Qt::Key_6:				return Key::Key6;
	case Qt::Key_7:				return Key::Key7;
	case Qt::Key_8:				return Key::Key8;
	case Qt::Key_9:				return Key::Key9;
	case Qt::Key_A:				return Key::A;
	case Qt::Key_B:				return Key::B;
	case Qt::Key_C:				return Key::C;
	case Qt::Key_D:				return Key::D;
	case Qt::Key_E:				return Key::E;
	case Qt::Key_F:				return Key::F;
	case Qt::Key_G:				return Key::G;
	case Qt::Key_H:				return Key::H;
	case Qt::Key_I:				return Key::I;
	case Qt::Key_J:				return Key::J;
	case Qt::Key_K:				return Key::K;
	case Qt::Key_L:				return Key::L;
	case Qt::Key_M:				return Key::M;
	case Qt::Key_N:				return Key::N;
	case Qt::Key_O:				return Key::O;
	case Qt::Key_P:				return Key::P;
	case Qt::Key_Q:				return Key::Q;
	case Qt::Key_R:				return Key::R;
	case Qt::Key_S:				return Key::S;
	case Qt::Key_T:				return Key::T;
	case Qt::Key_U:				return Key::U;
	case Qt::Key_V:				return Key::V;
	case Qt::Key_W:				return Key::W;
	case Qt::Key_X:				return Key::X;
	case Qt::Key_Y:				return Key::Y;
	case Qt::Key_Z:				return Key::Z;
	case Qt::Key_Semicolon:		return Key::Semicolon;
	case Qt::Key_Plus:			return Key::Add;
	case Qt::Key_Comma:			return Key::Comma;
	case Qt::Key_Minus:			return Key::Subtract;
	case Qt::Key_Period:		return Key::Period;
	case Qt::Key_Slash:			return Key::Slash;
	case Qt::Key_AsciiTilde:	return Key::Tilde;
	case Qt::Key_BracketLeft:	return Key::LeftBracket;
	case Qt::Key_Backslash:		return Key::BackSlash;
	case Qt::Key_BracketRight:	return Key::RightBracket;
	case Qt::Key_QuoteDbl:		return Key::Quote;

	case Qt::Key_Back:			return Key::Back;
	case Qt::Key_Tab:			return Key::Tab;
	case Qt::Key_Clear:			return Key::Clear;
	case Qt::Key_Return:		return Key::Return;
	case Qt::Key_Pause:			return Key::Pause;

	case Qt::Key_Escape:		return Key::Escape;

	case Qt::Key_End:			return Key::End;
	case Qt::Key_Home:			return Key::Home;
	case Qt::Key_Left:			return Key::Left;
	case Qt::Key_Up:			return Key::Up;
	case Qt::Key_Right:			return Key::Right;
	case Qt::Key_Down:			return Key::Down;

	case Qt::Key_Insert:		return Key::Insert;
	case Qt::Key_Delete:		return Key::Delete;

	case Qt::Key_F1:			return Key::F1;
	case Qt::Key_F2:			return Key::F2;
	case Qt::Key_F3:			return Key::F3;
	case Qt::Key_F4:			return Key::F4;
	case Qt::Key_F5:			return Key::F5;
	case Qt::Key_F6:			return Key::F6;
	case Qt::Key_F7:			return Key::F7;
	case Qt::Key_F8:			return Key::F8;
	case Qt::Key_F9:			return Key::F9;
	case Qt::Key_F10:			return Key::F10;
	case Qt::Key_F11:			return Key::F11;
	case Qt::Key_F12:			return Key::F12;
	case Qt::Key_F13:			return Key::F13;
	case Qt::Key_F14:			return Key::F14;
	case Qt::Key_F15:			return Key::F15;
	case Qt::Key_F16:			return Key::F16;
	case Qt::Key_F17:			return Key::F17;
	case Qt::Key_F18:			return Key::F18;
	case Qt::Key_F19:			return Key::F19;
	case Qt::Key_F20:			return Key::F20;
	case Qt::Key_F21:			return Key::F21;
	case Qt::Key_F22:			return Key::F22;
	case Qt::Key_F23:			return Key::F23;
	case Qt::Key_F24:			return Key::F24;

	case Qt::Key_NumLock:		return Key::NumLock;
	case Qt::Key_ScrollLock:	return Key::Scroll;

	case Qt::Key_Shift:			return Key::Shift;
	case Qt::Key_Control:		return Key::Control;
	case Qt::Key_Alt:			return Key::LeftMenu;
	case Qt::Key_AltGr:			return Key::RightMenu;
	case Qt::Key_Meta:			return Key::LeftWindows;

	default:
		break;
	}
	//n_error("Invalid key code!");
	return 0;
}

//------------------------------------------------------------------------------
/**
*/
bool
QtInputProxy::OnEvent(const Input::InputEvent& inputEvent)
{
	if (!this->ignore)
	{
		switch (inputEvent.GetType())
		{
		case Input::InputEvent::KeyDown:
			this->OnKeyDown(inputEvent);
			break;
		case Input::InputEvent::KeyUp:
			this->OnKeyUp(inputEvent);
			break;
		case Input::InputEvent::AppObtainFocus:
			this->OnGainFocus();
			break;
		}
	}      
	return false;
}

//------------------------------------------------------------------------------
/**
*/
bool
QtInputProxy::OnKeyDown(const Input::InputEvent& inputEvent)
{
    return SendKeyEvent(inputEvent);
}
    
//------------------------------------------------------------------------------
/**
*/
bool
QtInputProxy::OnKeyUp(const Input::InputEvent& inputEvent)
{
    return SendKeyEvent(inputEvent);
}

//------------------------------------------------------------------------------
/**
*/
void
QtInputProxy::SetReceiver(QObject* r)
{
    this->receiver = r;
}

//------------------------------------------------------------------------------
/**
*/
bool
QtInputProxy::SendKeyEvent(const Input::InputEvent& inputEvent)
{     
    // dont do modifiers on their own as it messes up shortcuts
    if((inputEvent.GetKey() == Input::Key::Control) || (inputEvent.GetKey() == Input::Key::LeftControl) || (inputEvent.GetKey() == Input::Key::RightControl)
       || (inputEvent.GetKey() == Input::Key::Shift) || (inputEvent.GetKey() == Input::Key::LeftShift) || (inputEvent.GetKey() == Input::Key::RightShift)
       || (inputEvent.GetKey() == Input::Key::Menu) || (inputEvent.GetKey() == Input::Key::LeftMenu) || (inputEvent.GetKey() == Input::Key::RightMenu) 
       || (inputEvent.GetKey() == Input::Key::LeftWindows) || (inputEvent.GetKey() == Input::Key::LeftWindows))
    {
        //return true;
    }
    
    // get modifers
    const Ptr<Input::Keyboard>& keyboard = InputServer::Instance()->GetDefaultKeyboard();
    unsigned mod = 0;
    
    if(keyboard->KeyPressed(Input::Key::Shift) || keyboard->KeyPressed(Input::Key::LeftShift) || keyboard->KeyPressed(Input::Key::RightShift))
    {        
        mod |= Qt::ShiftModifier;
    }
    if(keyboard->KeyPressed(Input::Key::Control) || keyboard->KeyPressed(Input::Key::LeftControl) || keyboard->KeyPressed(Input::Key::RightControl))
    {     
        mod |= Qt::ControlModifier;
    }
    if(keyboard->KeyPressed(Input::Key::Menu) || keyboard->KeyPressed(Input::Key::LeftMenu) || keyboard->KeyPressed(Input::Key::RightMenu))
    {     
        mod |= Qt::AltModifier;
    }
    if(keyboard->KeyPressed(Input::Key::LeftWindows) || keyboard->KeyPressed(Input::Key::RightWindows))
    {     
        mod |= Qt::MetaModifier;
    }
    
    QKeyEvent::Type kt;
    switch(inputEvent.GetType())
    {
        case InputEvent::KeyDown:
            kt = QEvent::KeyPress;
            break;
        case InputEvent::KeyUp:
            kt = QEvent::KeyRelease;
            break;
        default:
            n_error("unknown event\n");
            break;
    }
    
    QString ch = Key::ToString(inputEvent.GetKey()).AsCharPtr();
	QKeyEvent ev(kt, Neb2QtInputKey(inputEvent.GetKey()), (Qt::KeyboardModifier)mod, ch);

    QApplication::instance()->notify(this->receiver, &ev);
    return true;
}

//------------------------------------------------------------------------------
/**
*/
bool 
QtInputProxy::OnGainFocus()
{
    QWidget* widget = (QWidget*)this->receiver;    
	
	if (QApplication::focusWidget() != widget)
	{
		QFocusEvent ev(QEvent::FocusOut, Qt::OtherFocusReason);
		QApplication::instance()->notify(QApplication::focusWidget(), &ev);
		//widget->setFocus();
		//QApplication::focusWidget()->clearFocus();
	}
	// clear any current focus
	//

    // only raise main window if it's not focused already
    if (!widget->isActiveWindow())
    {
        widget->activateWindow();

    }    
    return true;
}

} // namespace QtFeature
//------------------------------------------------------------------------------