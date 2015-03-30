#pragma once
//------------------------------------------------------------------------------
/**
    @class Tools::ShellApplication
    
    Application class for Nebula3's command shell. Takes commands from
    stdin and evaluates them through the script server.

    FIXME: the Scripting subsystem has been removed from the Foundation
    layer and moved into an Addon! NSH3 needs to be fixed in order to work again!
    
    (C) 2006 Radon Labs GmbH
*/
#include "app/consoleapplication.h"

//------------------------------------------------------------------------------
namespace Tools
{
class ShellApplication : public App::ConsoleApplication
{
public:
    /// constructor
    ShellApplication();
    /// destructor
    virtual ~ShellApplication();
    /// run the application, return when user wants to exit
    virtual void Run();
};

} // namespace App
//------------------------------------------------------------------------------
