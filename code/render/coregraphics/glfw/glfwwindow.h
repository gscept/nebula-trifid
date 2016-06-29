#pragma once
//------------------------------------------------------------------------------
/**
	Implements a window using GLFW
	
	(C) 2016 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------
#include "coregraphics/base/windowbase.h"
#include "GLFW/glfw3.h"
namespace OpenGL4
{
class GLFWWindow : public Base::WindowBase
{
	__DeclareClass(GLFWWindow);
public:
	/// constructor
	GLFWWindow();
	/// destructor
	virtual ~GLFWWindow();

	/// open window
	void Open();
	/// close window
	void Close();
	/// reopen window (using new width and height)
	void Reopen();

	/// called when the view becomes current
	void MakeCurrent();

	/// swap buffers
	void SwapBuffers(IndexT frameIndex);

	/// set window title
	void SetTitle(const Util::String& title);
	/// set windowed/fullscreen mode
	void SetFullscreen(bool b, int monitor);
	/// set cursor visible
	void SetCursorVisible(bool visible);
	/// set cursor locked to window
	void SetCursorLocked(bool locked);

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
	friend class OpenGL4::OGL4RenderDevice;

	/// Keyboard callback
	void KeyFunc(int key, int scancode, int action, int mods);
	/// Character callback
	void CharFunc(unsigned int key);
	/// Mouse Button callback
	void MouseButtonFunc(int button, int action, int mods);
	/// Mouse callback
	void MouseFunc(double xpos, double ypos);
	/// Scroll callback
	void ScrollFunc(double xs, double ys);
	/// window close func
	void CloseFunc();
	/// window focus
	void FocusFunc(int focus);
	/// window resize
	void ResizeFunc(int width, int height);

	/// enables callbacks
	void EnableCallbacks();
	/// disables callbacks
	void DisableCallbacks();

	/// apply fullscreen
	void ApplyFullscreen();

	// not a pointer to self!
	GLFWwindow* window;
};

//------------------------------------------------------------------------------
/**
*/
inline void
GLFWWindow::SwapBuffers(IndexT frameIndex)
{
	n_assert(this->window != 0);
	if (this->swapFrame != frameIndex)
	{
		glfwSwapBuffers(this->window);
		this->swapFrame = frameIndex;
	}
}

} // namespace GLFW