//------------------------------------------------------------------------------
// glfwwindow.cc
// (C) 2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "glfwwindow.h"
#include "glfwdisplaydevice.h"
#include "input/key.h"
#include "coregraphics/displayevent.h"
#include "coregraphics/renderdevice.h"
#include "coregraphics/displaydevice.h"

using namespace CoreGraphics;
using namespace Math;
namespace OpenGL4
{

__ImplementClass(GLFWWindow, 'GFWW', Base::WindowBase);
//------------------------------------------------------------------------------
/**
*/
GLFWWindow::GLFWWindow() :
	window(NULL)
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
GLFWWindow::~GLFWWindow()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
void
GLFWWindow::Open()
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
	glfwWindowHint(GLFW_SRGB_CAPABLE, GL_TRUE);

	// get original window, this is so we don't have to create more contexts
	GLFWwindow* wnd = NULL;
	const Ptr<CoreGraphics::Window>& origWindow = CoreGraphics::DisplayDevice::Instance()->GetCurrentWindow();	
	if (origWindow.isvalid()) wnd = origWindow->window;
	
	// if we have window data, setup from alien window
	if (this->windowData.IsValid())
	{
		// create window using our Qt window as child
		this->window = glfwCreateWindowFromAlien(this->windowData.GetPtr(), wnd);
		glfwMakeContextCurrent(this->window);

		// get actual window size
		int height, width;
		glfwGetWindowSize(this->window, &width, &height);

		// update display mode
		this->displayMode.SetWidth(width);
		this->displayMode.SetHeight(height);
		this->displayMode.SetAspectRatio(width / float(height));

		// set user pointer to this window
		glfwSetWindowUserPointer(this->window, this);
	}
	else
	{
		glfwWindowHint(GLFW_RESIZABLE, this->resizable ? GL_TRUE : GL_FALSE);
		glfwWindowHint(GLFW_DECORATED, this->decorated ? GL_TRUE : GL_FALSE);

		// create window
		this->window = glfwCreateWindow(this->displayMode.GetWidth(), this->displayMode.GetHeight(), this->windowTitle.AsCharPtr(), NULL, wnd);
		if (!this->fullscreen) glfwSetWindowPos(this->window, this->displayMode.GetXPos(), this->displayMode.GetYPos());
		else				   this->ApplyFullscreen();

		glfwMakeContextCurrent(this->window);

		// set user pointer to this window
		glfwSetWindowUserPointer(this->window, this);
		glfwSetWindowTitle(this->window, this->windowTitle.AsCharPtr());
	}	

	// notify window is opened
	GLFWDisplayDevice::Instance()->NotifyEventHandlers(DisplayEvent(DisplayEvent::WindowOpen, this->windowId));

	// enable callbacks
	this->EnableCallbacks();
	WindowBase::Open();
}

//------------------------------------------------------------------------------
/**
*/
void
GLFWWindow::Close()
{
	this->DisableCallbacks();
	glfwDestroyWindow(this->window);

	// close event
	GLFWDisplayDevice::Instance()->NotifyEventHandlers(DisplayEvent(DisplayEvent::WindowClose, this->windowId));
	WindowBase::Close();
}

//------------------------------------------------------------------------------
/**
*/
void
GLFWWindow::Reopen()
{
	// just ignore full screen if this window is embedded
	if (!this->embedded)
	{
		// if we toggle full screen, select monitor (just selects primary for the moment) and apply full screen
		if (this->fullscreen)
		{
			this->ApplyFullscreen();
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

	// post event
	GLFWDisplayDevice::Instance()->NotifyEventHandlers(DisplayEvent(DisplayEvent::WindowReopen, this->windowId));

	// open window again
	WindowBase::Reopen();
}

//------------------------------------------------------------------------------
/**
*/
void
GLFWWindow::SetTitle(const Util::String& title)
{
	glfwSetWindowTitle(this->window, title.AsCharPtr());
}

//------------------------------------------------------------------------------
/**
*/
void
GLFWWindow::SetFullscreen(bool b, int monitor)
{
	WindowBase::SetFullscreen(b, monitor);
	if (this->window && b)
	{
		this->ApplyFullscreen();
	}
	else
	{
		glfwSetWindowMonitor(this->window, NULL, this->displayMode.GetWidth(), this->displayMode.GetHeight());
	}
}

//------------------------------------------------------------------------------
/**
*/
void
GLFWWindow::SetCursorVisible(bool visible)
{
	WindowBase::SetCursorVisible(visible);
	glfwSetInputMode(this->window, GLFW_CURSOR, visible ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_HIDDEN);
}

//------------------------------------------------------------------------------
/**
*/
void
GLFWWindow::SetCursorLocked(bool locked)
{
	WindowBase::SetCursorLocked(locked);
	glfwSetInputMode(this->window, GLFW_CURSOR, locked ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
}

//------------------------------------------------------------------------------
/**
callback for key events
*/
void
staticKeyFunc(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	GLFWWindow* wnd = (GLFWWindow*)glfwGetWindowUserPointer(window);
	wnd->KeyFunc(key, scancode, action, mods);
}

//------------------------------------------------------------------------------
/**
*/
void
GLFWWindow::KeyFunc(int key, int scancode, int action, int mods)
{
	Input::Key::Code keyCode = GLFWDisplayDevice::TranslateKeyCode(key);
	DisplayEvent::Code evtype;
	switch (action)
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
		GLFWDisplayDevice::Instance()->NotifyEventHandlers(DisplayEvent(evtype, keyCode));
	}
}


//------------------------------------------------------------------------------
/**
callbacks for character input
*/
void
staticCharFunc(GLFWwindow* window, unsigned int key)
{
	GLFWWindow* wnd = (GLFWWindow*)glfwGetWindowUserPointer(window);
	wnd->CharFunc(key);
}

//------------------------------------------------------------------------------
/**
*/
void
GLFWWindow::CharFunc(unsigned int key)
{
	GLFWDisplayDevice::Instance()->NotifyEventHandlers(DisplayEvent(DisplayEvent::Character, (Input::Char)key));
}

//------------------------------------------------------------------------------
/**
*/
void
staticMouseButtonFunc(GLFWwindow* window, int button, int action, int mods)
{
	GLFWWindow* wnd = (GLFWWindow*)glfwGetWindowUserPointer(window);
	wnd->MouseButtonFunc(button, action, mods);
}

//------------------------------------------------------------------------------
/**
callbacks for mouse buttons
*/
void
GLFWWindow::MouseButtonFunc(int button, int action, int mods)
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
	double x, y;
	glfwGetCursorPos(this->window, &x, &y);

	float2 pos;
	pos.set((float)x / float(this->displayMode.GetWidth()), (float)y / float(this->displayMode.GetHeight()));
	GLFWDisplayDevice::Instance()->NotifyEventHandlers(DisplayEvent(act, but, float2((float)x, (float)y), pos));
}

//------------------------------------------------------------------------------
/**
callbacks for mouse position
*/
void
staticMouseFunc(GLFWwindow* window, double xpos, double ypos)
{
	GLFWWindow* wnd = (GLFWWindow*)glfwGetWindowUserPointer(window);
	wnd->MouseFunc(xpos, ypos);
}

//------------------------------------------------------------------------------
/**
*/
void
GLFWWindow::MouseFunc(double xpos, double ypos)
{
	float2 absMousePos((float)xpos, (float)ypos);
	float2 pos;
	pos.set((float)xpos / float(this->displayMode.GetWidth()), (float)ypos / float(this->displayMode.GetHeight()));
	GLFWDisplayDevice::Instance()->NotifyEventHandlers(DisplayEvent(DisplayEvent::MouseMove, absMousePos, pos));
}

//------------------------------------------------------------------------------
/**
callbacks for scroll event
*/
void
staticScrollFunc(GLFWwindow* window, double xs, double ys)
{
	GLFWWindow* wnd = (GLFWWindow*)glfwGetWindowUserPointer(window);
	wnd->ScrollFunc(xs, ys);
}

//------------------------------------------------------------------------------
/**
callback for scroll events. only vertical scrolling is supported
and no scroll amounts are used, just single steps
*/
void
GLFWWindow::ScrollFunc(double xs, double ys)
{
	if (ys != 0.0)
	{
		GLFWDisplayDevice::Instance()->NotifyEventHandlers(DisplayEvent(ys > 0.0f ? DisplayEvent::MouseWheelForward : DisplayEvent::MouseWheelBackward));
	}
}

//------------------------------------------------------------------------------
/**
callback for close requested
*/
void
staticCloseFunc(GLFWwindow* window)
{
	GLFWWindow* wnd = (GLFWWindow*)glfwGetWindowUserPointer(window);
	wnd->CloseFunc();
}

//------------------------------------------------------------------------------
/**
*/
void
GLFWWindow::CloseFunc()
{
	GLFWDisplayDevice::Instance()->NotifyEventHandlers(DisplayEvent(DisplayEvent::CloseRequested, this->windowId));
}

//------------------------------------------------------------------------------
/**
callback for focus
*/
void
staticFocusFunc(GLFWwindow* window, int focus)
{
	GLFWWindow* wnd = (GLFWWindow*)glfwGetWindowUserPointer(window);
	wnd->FocusFunc(focus);
}

//------------------------------------------------------------------------------
/**
*/
void
GLFWWindow::FocusFunc(int focus)
{
	if (focus)
	{
		GLFWDisplayDevice::Instance()->NotifyEventHandlers(DisplayEvent(DisplayEvent::SetFocus, this->windowId));
	}
	else
	{
		GLFWDisplayDevice::Instance()->NotifyEventHandlers(DisplayEvent(DisplayEvent::KillFocus, this->windowId));
	}
}

//------------------------------------------------------------------------------
/**
*/
void
staticSizeFunc(GLFWwindow* window, int width, int height)
{
	GLFWWindow* wnd = (GLFWWindow*)glfwGetWindowUserPointer(window);
	wnd->ResizeFunc(width, height);
}

//------------------------------------------------------------------------------
/**
*/
void
GLFWWindow::ResizeFunc(int width, int height)
{
	// only resize if size is not 0
	if (width != 0 && height != 0)
	{
		this->displayMode.SetWidth(width);
		this->displayMode.SetHeight(height);
		this->displayMode.SetAspectRatio(width / float(height));

		// resize default render target
		this->Resize(width, height);

		// notify event listeners we resized
		GLFWDisplayDevice::Instance()->NotifyEventHandlers(DisplayEvent(DisplayEvent::WindowResized, this->windowId));
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
GLFWWindow::EnableCallbacks()
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
GLFWWindow::DisableCallbacks()
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

//------------------------------------------------------------------------------
/**
*/
void
GLFWWindow::ApplyFullscreen()
{
	GLFWmonitor* mon;
	if (monitor == -1)
	{
		mon = glfwGetPrimaryMonitor();
	}
	else
	{
		int count;
		GLFWmonitor** monitors = glfwGetMonitors(&count);
		n_assert(monitor < count);
		mon = monitors[monitor];
	}
	const GLFWvidmode* mode = glfwGetVideoMode(mon);
	glfwSetWindowMonitor(this->window, mon, mode->width, mode->height);
}

//------------------------------------------------------------------------------
/**
*/
void
GLFWWindow::MakeCurrent()
{
	glfwMakeContextCurrent(this->window);
}

} // namespace OpenGL4