#pragma once
//------------------------------------------------------------------------------
/**
    @class OpenGL4::GLFWInputServer
    
    glfw-specific InputServer (provides a default Keyboard and Mouse).
        
    (C) 2013-2015 Individual contributors, see AUTHORS file
*/
#include "core/singleton.h"
#include "input/base/inputserverbase.h"
#include "input/glfw/glfwinputdisplayeventhandler.h"

//------------------------------------------------------------------------------
namespace OpenGL4
{
class GLFWInputServer : public Base::InputServerBase
{
    __DeclareClass(GLFWInputServer);
    __DeclareSingleton(GLFWInputServer);
public:
    /// constructor
    GLFWInputServer();
    /// destructor
    virtual ~GLFWInputServer();

    /// open the input server
    void Open();
    /// close the input server
    void Close();
    /// call after processing window events 
    void OnFrame();
    /// set visibility of system cursor
    void SetCursorVisible(bool enable);
	/// lock cursor and hide it
	void SetCursorLocked(bool enable);

protected:    

    Ptr<GLFWInputDisplayEventHandler> eventHandler;
};


} // namespace OpenGL4
//------------------------------------------------------------------------------