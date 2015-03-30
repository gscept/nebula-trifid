#pragma once
//------------------------------------------------------------------------------
/**
    @class OpenGL4::GLFWDisplayDevice

    GLFW based OpenGL implementation of DisplayDevice class.

    (C) 2013 Johannes Hirche
*/
#include "coregraphics/base/displaydevicebase.h"
#include "util/array.h"
#include "threading/thread.h"



//------------------------------------------------------------------------------
namespace OpenGL4
{
class GLFWDisplayDevice : public Base::DisplayDeviceBase
{
    __DeclareClass(GLFWDisplayDevice);
    __DeclareSingleton(GLFWDisplayDevice);
public:
    /// constructor
    GLFWDisplayDevice();
    /// destructor
    virtual ~GLFWDisplayDevice();
	/// return true if adapter exists
	bool AdapterExists(CoreGraphics::Adapter::Code adapter);
	/// get available display modes on given adapter
	Util::Array<CoreGraphics::DisplayMode> GetAvailableDisplayModes(CoreGraphics::Adapter::Code adapter, CoreGraphics::PixelFormat::Code pixelFormat);
	/// return true if a given display mode is supported
	bool SupportsDisplayMode(CoreGraphics::Adapter::Code adapter, const CoreGraphics::DisplayMode& requestedMode);
	/// get current adapter display mode (i.e. the desktop display mode)
	CoreGraphics::DisplayMode GetCurrentAdapterDisplayMode(CoreGraphics::Adapter::Code adapter);
	/// get general info about display adapter
	CoreGraphics::AdapterInfo GetAdapterInfo(CoreGraphics::Adapter::Code adapter);

	/// set window title string (can be changed anytime)
	void SetWindowTitle(const Util::String& t);

	/// open the display
	bool Open();
	/// close the display
	void Close();
	/// process window system messages, call this method once per frame
	static void ProcessWindowMessages();
	/// reopens the display device which enables switching from display modes
	void Reopen();
    /// enables callbacks
    void EnableCallbacks();
    /// disables callbacks
    void DisableCallbacks();

	/// swap buffers
	void SwapBuffers();

	/// declare static key function as friend
	friend void staticKeyFunc(GLFWwindow *window, int key, int scancode, int action, int mods);
	/// declare static mouse button function as friend
	friend void staticMouseButtonFunc(GLFWwindow *window, int button, int action, int mods);
	/// declare static mouse function as friend
	friend void staticMouseFunc(GLFWwindow *window, double xpos, double ypos);
    /// declare static scroll function as friend
    friend void staticScrollFunc(GLFWwindow *window, double xs, double ys);
	/// declare static close function as friend
	friend void staticCloseFunc(GLFWwindow * window);
	/// declare static focus function as friend
	friend void staticFocusFunc(GLFWwindow * window, int focus);
	/// declare static size function as friend
	friend void staticSizeFunc(GLFWwindow* window, int width, int height);
	/// declare static char function as friend
	friend void staticCharFunc(GLFWwindow* window, unsigned int key);

protected:

	/// Keyboard callback
	void KeyFunc(GLFWwindow *window, int key, int scancode, int action, int mods);
	/// Character callback
	void CharFunc(GLFWwindow *window, unsigned int key);
	/// Mouse Button callback
	void MouseButtonFunc(GLFWwindow *window, int button, int action, int mods);
	/// Mouse callback
	void MouseFunc(GLFWwindow *window, double xpos, double ypos);
    /// Scroll callback
    void ScrollFunc(GLFWwindow *window, double xs, double ys);
	/// window close func
	void CloseFunc(GLFWwindow *window);
	/// window focus
	void FocusFunc(GLFWwindow* window, int);
	/// window resize
	void ResizeFunc(GLFWwindow* window, int width, int height);	

	friend class OGL4RenderDevice;
    friend class GLFWInputServer;
	/// open window
	bool OpenWindow();
	/// open embedded window
	bool EmbedWindow();
	/// opaque glfw data structure
	GLFWwindow * window;
	/// retrieve monitor from adapter. can be NULL
	GLFWmonitor * GetMonitor(CoreGraphics::Adapter::Code a);
	/// translate glfw keycodes to nebula ones
	Input::Key::Code TranslateKeyCode(int inkey) const;

};


//------------------------------------------------------------------------------
/**
    swaps buffers
*/
inline void
GLFWDisplayDevice::SwapBuffers()
{
	n_assert(this->IsOpen());
	glfwSwapBuffers(this->window);

}

} // namespace OpenGL4
//------------------------------------------------------------------------------
