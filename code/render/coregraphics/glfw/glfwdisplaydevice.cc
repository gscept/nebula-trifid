//------------------------------------------------------------------------------
//	glfwdisplaydevice.cc
//  (C) 2013-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------

#include "stdneb.h"
#include "coregraphics/config.h"
#include "coregraphics/glfw/glfwdisplaydevice.h"
#include "coregraphics/ogl4/ogl4renderdevice.h"
#include "coregraphics/ogl4/ogl4types.h"
#include "coregraphics/renderdevice.h"
#include "GLFW/glfw3native.h"
#include <GLFW/glfw3.h>

#if __WIN32__
#include "events/win32/win32event.h"

// Forward-declare GLFW windowProc
static LRESULT CALLBACK windowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

#endif


#if NEBULA3_OPENGL4_DEBUG
//------------------------------------------------------------------------------
/**
*/
void
NebulaGLFWErrorCallback(int errcode, const char* msg)
{
	n_error("GL ERROR: code %d, %s", errcode, msg);
}

#endif


namespace OpenGL4
{
__ImplementClass(OpenGL4::GLFWDisplayDevice, 'O4WD', Base::DisplayDeviceBase);
__ImplementSingleton(OpenGL4::GLFWDisplayDevice);

using namespace Util;
using namespace Math;
using namespace CoreGraphics;
using namespace OpenGL4;

//------------------------------------------------------------------------------
/**
*/
GLFWDisplayDevice::GLFWDisplayDevice() 	: 
	window(NULL)
{
    __ConstructSingleton;
	glfwInit();	
}

//------------------------------------------------------------------------------
/**
*/
GLFWDisplayDevice::~GLFWDisplayDevice()
{
    __DestructSingleton;
	glfwTerminate();
}

//------------------------------------------------------------------------------
/**
    Set the window title. An application should be able to change the
    window title at any time, that's why this is a virtual method, so that
    a subclass may override it.
*/
void
GLFWDisplayDevice::SetWindowTitle(const Util::String& str)
{	
    this->windowTitle = str;
	if(this->window)
	{
		glfwSetWindowTitle(this->window, str.AsCharPtr());
	}	
}

//------------------------------------------------------------------------------
/**
    Open the display.
*/
bool
GLFWDisplayDevice::Open()
{
    n_assert(!this->IsOpen());

	if (DisplayDeviceBase::Open())
	{
		bool success;
		if (this->embedded)
		{
			success = this->EmbedWindow();
		}
		else
		{
			success = this->OpenWindow();
		}

		if(success)
		{
			this->EnableCallbacks();
		}

		return success;
	}
	return false;
}

//------------------------------------------------------------------------------
/**
    Close the display.
*/
void
GLFWDisplayDevice::Close()
{
	n_assert(this->IsOpen());

	glfwDestroyWindow(this->window);
	this->window = NULL;
	DisplayDeviceBase::Close();
}

//------------------------------------------------------------------------------
/**
*/
void 
GLFWDisplayDevice::Reopen()
{
	n_assert(this->IsOpen());

    // just ignore full screen if this window is embedded
    if (!this->embedded)
    {
		// if we toggle full screen, select monitor (just selects primary for the moment) and apply full screen
		if (this->fullscreen)
		{
			GLFWmonitor* monitor = glfwGetPrimaryMonitor();
			const GLFWvidmode* mode = glfwGetVideoMode(monitor);
			glfwSetWindowMonitor(this->window, monitor, mode->width, mode->height);
			this->displayMode.SetWidth(mode->width);
			this->displayMode.SetHeight(mode->height);
		}
		else
		{
			// if not, set the window state and detach from the monitor
			glfwSetWindowMonitor(this->window, NULL, this->displayMode.GetWidth(), this->displayMode.GetHeight());
		}
    }    

    // only move window if not fullscreen
    if (!this->fullscreen)
    {
	    // update window with new size and position
	    glfwSetWindowPos(this->window, this->displayMode.GetXPos(), this->displayMode.GetYPos());
		glfwSetWindowSize(this->window, this->displayMode.GetWidth(), this->displayMode.GetHeight());
    }	
		
	DisplayDeviceBase::Reopen();	
}

//------------------------------------------------------------------------------
/**
    Process window system messages. Override this method in a subclass.
*/
void
GLFWDisplayDevice::ProcessWindowMessages()
{
	glfwPollEvents();
}

//------------------------------------------------------------------------------
/**
*/
bool
MatchPixelFormat(PixelFormat::Code format, const GLFWvidmode & mode)
{
	//FIXME this is just a subset
	switch(format)
	{
		case PixelFormat::X8R8G8B8:
		case PixelFormat::A8R8G8B8:
		case PixelFormat::SRGBA8:
		case PixelFormat::R8G8B8:
			return (mode.blueBits == 8 ) && (mode.redBits == 8 ) && (mode.greenBits == 8);
		break;
		case PixelFormat::R5G6B5:
			return (mode.blueBits == 5) && (mode.redBits == 5) && (mode.greenBits == 6);
		default:
			return false;
	}
	return false;
}

//------------------------------------------------------------------------------
/**
    Returns the display modes on the given adapter in the given pixel format.
*/
Util::Array<DisplayMode>
GLFWDisplayDevice::GetAvailableDisplayModes(Adapter::Code adapter, PixelFormat::Code pixelFormat)
{    
    Util::Array<DisplayMode> ret;
	GLFWmonitor * monitor = GetMonitor(adapter);
	
	if(monitor)
	{
		int count;
		const GLFWvidmode * modes = glfwGetVideoModes(monitor, &count);
		for(int i = 0 ; i < count ; i++)
		{
			if(MatchPixelFormat(pixelFormat, modes[i]))
			{
				DisplayMode mode(modes->width, modes->height, pixelFormat);
				ret.Append(mode);
			}
		}	
	}
    return ret;
}

//------------------------------------------------------------------------------
/**
    This method checks the available display modes on the given adapter
    against the requested display modes and returns true if the display mode
    exists.
*/
bool
GLFWDisplayDevice::SupportsDisplayMode(Adapter::Code adapter, const DisplayMode& requestedMode)
{	
	//FIXME only checks for width/height
	Util::Array<DisplayMode> modes = GetAvailableDisplayModes(adapter, requestedMode.GetPixelFormat());
	for(Util::Array<DisplayMode>::Iterator iter = modes.Begin();iter != modes.End();iter++)
	{
		if((requestedMode.GetHeight() == iter->GetHeight()) && (requestedMode.GetWidth() == iter->GetWidth()))
			return true;
	}
	return false;
}


//------------------------------------------------------------------------------
/**
*/
GLFWmonitor *
GLFWDisplayDevice::GetMonitor(Adapter::Code adapter)
{
	GLFWmonitor * monitor = NULL;

    switch(adapter)
	{
		case Adapter::Primary:
			{
				return glfwGetPrimaryMonitor();
			}
		break;
		default:
			{
				// grab one of the others
				int count;
				GLFWmonitor** monitors = glfwGetMonitors(&count);
				n_assert(count > 1);
				// glfw stores primary in first element
				monitor = monitors[1];
			}			
		break;
	}
	return monitor;
}

//------------------------------------------------------------------------------
/**
    This method returns the current adapter display mode. It can be used
    to get the current desktop display mode.
*/
DisplayMode
GLFWDisplayDevice::GetCurrentAdapterDisplayMode(Adapter::Code adapter)
{
    GLFWmonitor * monitor = GetMonitor(adapter);
	n_assert(monitor);
	const GLFWvidmode * mode = glfwGetVideoMode(monitor);
	PixelFormat::Code format;
	//FIXME
	if((mode->greenBits == 8 ) && ( mode->blueBits == 8 ) && (mode->redBits == 8))
	{
		format = PixelFormat::A8B8G8R8;
	}
	else
	{
		format = PixelFormat::InvalidPixelFormat;
	}
	
	DisplayMode dmode(mode->width, mode->height, format);
	dmode.SetRefreshRate(mode->refreshRate);
	return dmode;
}

//------------------------------------------------------------------------------
/**
    Checks if the given adapter exists.
*/
bool
GLFWDisplayDevice::AdapterExists(Adapter::Code adapter)
{
    return NULL != GetMonitor(adapter);
}

//------------------------------------------------------------------------------
/**
    Returns information about the provided adapter.
*/
AdapterInfo
GLFWDisplayDevice::GetAdapterInfo(Adapter::Code adapter)
{
    AdapterInfo emptyAdapterInfo;
    return emptyAdapterInfo;
}

//------------------------------------------------------------------------------
/**
    Opens a window
*/
bool
GLFWDisplayDevice::OpenWindow()
{	
#if NEBULA3_OPENGL4_DEBUG
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
	glfwSetErrorCallback(NebulaGLFWErrorCallback);    
    n_printf("Creating OpenGL debug context\n");
#else
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_FALSE);
	glDisable(GL_DEBUG_OUTPUT);
#endif
	glfwWindowHint(GLFW_RED_BITS, 8);
	glfwWindowHint(GLFW_GREEN_BITS, 8);
	glfwWindowHint(GLFW_BLUE_BITS, 8);
    glfwWindowHint(GLFW_RESIZABLE, this->resizable ? GL_TRUE : GL_FALSE);
    glfwWindowHint(GLFW_DECORATED, this->decorated ? GL_TRUE : GL_FALSE);
	glfwWindowHint(GLFW_SRGB_CAPABLE, GL_TRUE);

	// create window
	this->window = glfwCreateWindow(this->displayMode.GetWidth(), this->displayMode.GetHeight(), this->windowTitle.AsCharPtr(), this->fullscreen ? glfwGetPrimaryMonitor() : NULL, NULL);
	if (!this->fullscreen) glfwSetWindowPos(this->window, this->displayMode.GetXPos(), this->displayMode.GetYPos());
	glfwMakeContextCurrent(this->window);

    if (this->verticalSync)
    {
        glfwSwapInterval(1);
    }
	else
	{
		glfwSwapInterval(0);
	}

	return true;
}

//------------------------------------------------------------------------------
/**
    Embed a window from some other source (for example Qt)
*/
bool
GLFWDisplayDevice::EmbedWindow()
{
    n_assert(0 != this->windowData.GetPtr());
#if NEBULA3_OPENGL4_DEBUG
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
	glfwSetErrorCallback(NebulaGLFWErrorCallback);
	n_printf("Creating OpenGL debug context\n");
#else
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_FALSE);
	glDisable(GL_DEBUG_OUTPUT);
#endif
	glfwWindowHint(GLFW_RED_BITS, 8);
	glfwWindowHint(GLFW_GREEN_BITS, 8);
	glfwWindowHint(GLFW_BLUE_BITS, 8);
	glfwWindowHint(GLFW_SRGB_CAPABLE, GL_TRUE);

	// create window using our Qt window as child
	this->window = glfwCreateWindowFromAlien(this->windowData.GetPtr());
	glfwMakeContextCurrent(this->window);

    // get actual window size
    int height, width;
    glfwGetWindowSize(this->window, &width, &height);

    // update display mode
    this->displayMode.SetWidth(width);
    this->displayMode.SetHeight(height);
	this->displayMode.SetAspectRatio(height / float(width));

    if (this->verticalSync)
    {
        glfwSwapInterval(1);
    }
	else
	{
		glfwSwapInterval(0);
	}

	return true;
}

//------------------------------------------------------------------------------
/**
    translate keycode
*/
Input::Key::Code 
GLFWDisplayDevice::TranslateKeyCode(int inkey) const
{
	switch (inkey)
	{
	case GLFW_KEY_BACKSPACE:                   return Input::Key::Back;
	case GLFW_KEY_TAB:                    return Input::Key::Tab;	
	case GLFW_KEY_ENTER:                 return Input::Key::Return;			
	case GLFW_KEY_MENU:                   return Input::Key::Menu;
	case GLFW_KEY_PAUSE:                  return Input::Key::Pause;
	case GLFW_KEY_CAPS_LOCK:                return Input::Key::Capital;
	case GLFW_KEY_ESCAPE:                 return Input::Key::Escape;				
	case GLFW_KEY_SPACE:                  return Input::Key::Space;		
	case GLFW_KEY_END:                    return Input::Key::End;
	case GLFW_KEY_HOME:                   return Input::Key::Home;
	case GLFW_KEY_LEFT:                   return Input::Key::Left;
	case GLFW_KEY_RIGHT:                  return Input::Key::Right;
	case GLFW_KEY_UP:                     return Input::Key::Up;
	case GLFW_KEY_DOWN:                   return Input::Key::Down;	
	case GLFW_KEY_INSERT:                 return Input::Key::Insert;
	case GLFW_KEY_DELETE:                 return Input::Key::Delete;	
	case GLFW_KEY_LEFT_SUPER:                   return Input::Key::LeftWindows;
	case GLFW_KEY_RIGHT_SUPER:                   return Input::Key::RightWindows;	
	case GLFW_KEY_KP_0:                return Input::Key::NumPad0;
	case GLFW_KEY_KP_1:                return Input::Key::NumPad1;
	case GLFW_KEY_KP_2:                return Input::Key::NumPad2;
	case GLFW_KEY_KP_3:                return Input::Key::NumPad3;
	case GLFW_KEY_KP_4:                return Input::Key::NumPad4;
	case GLFW_KEY_KP_5:                return Input::Key::NumPad5;
	case GLFW_KEY_KP_6:                return Input::Key::NumPad6;
	case GLFW_KEY_KP_7:                return Input::Key::NumPad7;
	case GLFW_KEY_KP_8:                return Input::Key::NumPad8;
	case GLFW_KEY_KP_9:                return Input::Key::NumPad9;
	case GLFW_KEY_KP_MULTIPLY:               return Input::Key::Multiply;
	case GLFW_KEY_KP_ADD:                    return Input::Key::Add;
	case GLFW_KEY_KP_SUBTRACT:				return Input::Key::Subtract;
	case GLFW_KEY_COMMA:				return Input::Key::Comma;
	case GLFW_KEY_PERIOD:				return Input::Key::Period;
	case GLFW_KEY_APOSTROPHE:              return Input::Key::Separator;
	case GLFW_KEY_KP_DECIMAL:                return Input::Key::Decimal;
	case GLFW_KEY_KP_DIVIDE:                 return Input::Key::Divide;
	case GLFW_KEY_F1:                     return Input::Key::F1;
	case GLFW_KEY_F2:                     return Input::Key::F2;
	case GLFW_KEY_F3:                     return Input::Key::F3;
	case GLFW_KEY_F4:                     return Input::Key::F4;
	case GLFW_KEY_F5:                     return Input::Key::F5;
	case GLFW_KEY_F6:                     return Input::Key::F6;
	case GLFW_KEY_F7:                     return Input::Key::F7;
	case GLFW_KEY_F8:                     return Input::Key::F8;
	case GLFW_KEY_F9:                     return Input::Key::F9;
	case GLFW_KEY_F10:                    return Input::Key::F10;
	case GLFW_KEY_F11:                    return Input::Key::F11;
	case GLFW_KEY_F12:                    return Input::Key::F12;
	case GLFW_KEY_F13:                    return Input::Key::F13;
	case GLFW_KEY_F14:                    return Input::Key::F14;
	case GLFW_KEY_F15:                    return Input::Key::F15;
	case GLFW_KEY_F16:                    return Input::Key::F16;
	case GLFW_KEY_F17:                    return Input::Key::F17;
	case GLFW_KEY_F18:                    return Input::Key::F18;
	case GLFW_KEY_F19:                    return Input::Key::F19;
	case GLFW_KEY_F20:                    return Input::Key::F20;
	case GLFW_KEY_F21:                    return Input::Key::F21;
	case GLFW_KEY_F22:                    return Input::Key::F22;
	case GLFW_KEY_F23:                    return Input::Key::F23;
	case GLFW_KEY_F24:                    return Input::Key::F24;
	case GLFW_KEY_NUM_LOCK:                return Input::Key::NumLock;
	case GLFW_KEY_SCROLL_LOCK:                 return Input::Key::Scroll;
	case GLFW_KEY_LEFT_SHIFT:                 return Input::Key::LeftShift;
	case GLFW_KEY_RIGHT_SHIFT:                 return Input::Key::RightShift;
	case GLFW_KEY_LEFT_CONTROL:               return Input::Key::LeftControl;
	case GLFW_KEY_RIGHT_CONTROL:               return Input::Key::RightControl;
	case GLFW_KEY_LEFT_ALT:                  return Input::Key::LeftMenu;
	case GLFW_KEY_RIGHT_ALT:                  return Input::Key::RightMenu;	
	//case VK_OEM_3:					return Input::Key::Tilde;
	case GLFW_KEY_0:                       return Input::Key::Key0;
	case GLFW_KEY_1:                       return Input::Key::Key1;
	case GLFW_KEY_2:                       return Input::Key::Key2;
	case GLFW_KEY_3:                       return Input::Key::Key3;
	case GLFW_KEY_4:                       return Input::Key::Key4;
	case GLFW_KEY_5:                       return Input::Key::Key5;
	case GLFW_KEY_6:                       return Input::Key::Key6;
	case GLFW_KEY_7:                       return Input::Key::Key7;
	case GLFW_KEY_8:                       return Input::Key::Key8;
	case GLFW_KEY_9:                       return Input::Key::Key9;
	case GLFW_KEY_A:                       return Input::Key::A;
	case GLFW_KEY_B:                       return Input::Key::B;
	case GLFW_KEY_C:                       return Input::Key::C;
	case GLFW_KEY_D:                       return Input::Key::D;
	case GLFW_KEY_E:                       return Input::Key::E;
	case GLFW_KEY_F:                       return Input::Key::F;
	case GLFW_KEY_G:                       return Input::Key::G;
	case GLFW_KEY_H:                       return Input::Key::H;
	case GLFW_KEY_I:                       return Input::Key::I;
	case GLFW_KEY_J:                       return Input::Key::J;
	case GLFW_KEY_K:                       return Input::Key::K;
	case GLFW_KEY_L:                       return Input::Key::L;
	case GLFW_KEY_M:                       return Input::Key::M;
	case GLFW_KEY_N:                       return Input::Key::N;
	case GLFW_KEY_O:                       return Input::Key::O;
	case GLFW_KEY_P:                       return Input::Key::P;
	case GLFW_KEY_Q:                       return Input::Key::Q;
	case GLFW_KEY_R:                       return Input::Key::R;
	case GLFW_KEY_S:                       return Input::Key::S;
	case GLFW_KEY_T:                       return Input::Key::T;
	case GLFW_KEY_U:                       return Input::Key::U;
	case GLFW_KEY_V:                       return Input::Key::V;
	case GLFW_KEY_W:                       return Input::Key::W;
	case GLFW_KEY_X:                       return Input::Key::X;
	case GLFW_KEY_Y:                       return Input::Key::Y;
	case GLFW_KEY_Z:                       return Input::Key::Z;
	default:                        return Input::Key::InvalidKey;
	}
	return Input::Key::InvalidKey;
}


//------------------------------------------------------------------------------
/**
    callback for key events
*/
void
staticKeyFunc(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    GLFWDisplayDevice::Instance()->KeyFunc(window, key, scancode, action, mods);
}

//------------------------------------------------------------------------------
/**
*/
void 
GLFWDisplayDevice::KeyFunc(GLFWwindow *window, int key, int scancode, int action, int mods)
{
	Input::Key::Code keyCode = TranslateKeyCode(key);
	DisplayEvent::Code evtype;
	switch(action)
	{
    case GLFW_REPEAT:
	case GLFW_PRESS:
		evtype = DisplayEvent::KeyDown;
		break;
	case GLFW_RELEASE:
		evtype = DisplayEvent::KeyUp;
		break;
	default:
		return;
	}
	if (Input::Key::InvalidKey != keyCode)
	{
		this->NotifyEventHandlers(DisplayEvent(evtype, keyCode));
	}
}


//------------------------------------------------------------------------------
/**
    callbacks for character input
*/
void
staticCharFunc(GLFWwindow *window, unsigned int key)
{
    GLFWDisplayDevice::Instance()->CharFunc(window, key);
}

//------------------------------------------------------------------------------
/**
*/
void 
GLFWDisplayDevice::CharFunc(GLFWwindow *window, unsigned int key)
{
	this->NotifyEventHandlers(DisplayEvent(DisplayEvent::Character, key));
}

//------------------------------------------------------------------------------
/**
    callbacks for mouse buttons
*/
void 
GLFWDisplayDevice::MouseButtonFunc(GLFWwindow *window, int button, int action, int mods)
{
	DisplayEvent::Code act = action == GLFW_PRESS ? DisplayEvent::MouseButtonDown : DisplayEvent::MouseButtonUp;
	Input::MouseButton::Code but;
	switch (button)
	{
	case GLFW_MOUSE_BUTTON_LEFT:
		but = Input::MouseButton::LeftButton;
		break;
	case GLFW_MOUSE_BUTTON_RIGHT:
		but = Input::MouseButton::RightButton;
		break;
	case GLFW_MOUSE_BUTTON_MIDDLE:
		but = Input::MouseButton::MiddleButton;
		break;
	default:
		return;
	}
	double x,y;
    glfwGetCursorPos(this->window, &x, &y);

	float2 pos;
	pos.set((float)x / float(this->displayMode.GetWidth()), (float)y / float(this->displayMode.GetHeight()));
    this->NotifyEventHandlers(DisplayEvent(act, but, float2((float)x, (float)y), pos));
}

//------------------------------------------------------------------------------
/**
*/
void
staticMouseButtonFunc(GLFWwindow *window, int button, int action, int mods)
{
    GLFWDisplayDevice::Instance()->MouseButtonFunc(window, button, action, mods);
}

//------------------------------------------------------------------------------
/**
    callbacks for mouse position
*/
void
staticMouseFunc(GLFWwindow *window, double xpos, double ypos)
{
    GLFWDisplayDevice::Instance()->MouseFunc(window, xpos, ypos);
}

//------------------------------------------------------------------------------
/**
*/
void 
GLFWDisplayDevice::MouseFunc(GLFWwindow *window, double xpos, double ypos)
{
	float2 absMousePos((float)xpos,(float)ypos);
	float2 pos;
	pos.set((float)xpos / float(this->displayMode.GetWidth()), (float)ypos / float(this->displayMode.GetHeight()));
	this->NotifyEventHandlers(DisplayEvent(DisplayEvent::MouseMove, absMousePos, pos));
}

//------------------------------------------------------------------------------
/**
    callbacks for scroll event
*/
void
staticScrollFunc(GLFWwindow *window, double xs, double ys)
{
    GLFWDisplayDevice::Instance()->ScrollFunc(window, xs, ys);
}

//------------------------------------------------------------------------------
/**
    callback for scroll events. only vertical scrolling is supported 
    and no scroll amounts are used, just single steps
*/
void 
GLFWDisplayDevice::ScrollFunc(GLFWwindow *window, double xs, double ys)
{
	if(ys != 0.0)
    {
	    this->NotifyEventHandlers(DisplayEvent(ys > 0.0f ? DisplayEvent::MouseWheelForward : DisplayEvent::MouseWheelBackward));
    }
}

//------------------------------------------------------------------------------
/**
    callback for close requested
*/
void
staticCloseFunc(GLFWwindow * window)
{
	GLFWDisplayDevice::Instance()->CloseFunc(window);
}

//------------------------------------------------------------------------------
/**
*/
void 
GLFWDisplayDevice::CloseFunc(GLFWwindow* window)
{
	this->NotifyEventHandlers(DisplayEvent(DisplayEvent::CloseRequested));
}

//------------------------------------------------------------------------------
/**
    callback for focus
*/
void
staticFocusFunc(GLFWwindow * window, int focus)
{
    GLFWDisplayDevice::Instance()->FocusFunc(window, focus);;
}

//------------------------------------------------------------------------------
/**
*/
void
GLFWDisplayDevice::FocusFunc(GLFWwindow* window, int focus)
{
	if(focus)
	{
		this->NotifyEventHandlers(DisplayEvent(DisplayEvent::SetFocus));
	}
	else
	{
		this->NotifyEventHandlers(DisplayEvent(DisplayEvent::KillFocus));
	}
}

//------------------------------------------------------------------------------
/**
*/
void
staticSizeFunc(GLFWwindow* window, int width, int height)
{
	GLFWDisplayDevice::Instance()->ResizeFunc(window, width, height);
}

//------------------------------------------------------------------------------
/**
*/
void 
GLFWDisplayDevice::ResizeFunc( GLFWwindow* window, int width, int height )
{
	// only resize if size is not 0
	if (width != 0 && height != 0)
	{
		this->displayMode.SetWidth(width);
		this->displayMode.SetHeight(height);
		this->displayMode.SetAspectRatio(width / float(height));

		// resize render targets and such
		DisplayDeviceBase::Reopen();
	}	
}

//------------------------------------------------------------------------------
/**
*/
void
staticDropFunc(GLFWwindow* window, int files, const char** args)
{
	// empty for now
}

//------------------------------------------------------------------------------
/**
    Enables callbacks through glfw
*/
void 
GLFWDisplayDevice::EnableCallbacks()
{
    glfwSetKeyCallback(this->window, staticKeyFunc);
    glfwSetMouseButtonCallback(this->window, staticMouseButtonFunc);
    glfwSetCursorPosCallback(this->window, staticMouseFunc);
    glfwSetWindowCloseCallback(this->window, staticCloseFunc);
    glfwSetWindowFocusCallback(this->window, staticFocusFunc);
    glfwSetWindowSizeCallback(this->window, staticSizeFunc);
    glfwSetScrollCallback(this->window, staticScrollFunc);
	glfwSetCharCallback(this->window, staticCharFunc);
	glfwSetDropCallback(this->window, staticDropFunc);
}

//------------------------------------------------------------------------------
/**
    Disables callbacks through glfw
*/
void 
GLFWDisplayDevice::DisableCallbacks()
{
    glfwSetKeyCallback(this->window, NULL);
    glfwSetMouseButtonCallback(this->window, NULL);
    glfwSetCursorPosCallback(this->window, NULL);
    glfwSetWindowCloseCallback(this->window, NULL);
    glfwSetWindowFocusCallback(this->window, NULL);
    glfwSetWindowSizeCallback(this->window, NULL);
    glfwSetScrollCallback(this->window, NULL);
	glfwSetCharCallback(this->window, NULL);
}

} // namespace CoreGraphics

