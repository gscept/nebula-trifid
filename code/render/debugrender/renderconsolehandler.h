#pragma once
//------------------------------------------------------------------------------
/**
    @class Render::RenderConsoleHandler
    
    Outputs the console. Saves all output texts which go to the standard console 
    and renders it on screen.
    Allows scrolling with up and down arrow keys.
    
    (C) 2008 Radon Labs GmbH
    (C) 2013-2015 Individual contributors, see AUTHORS file
*/
#include "io/historyconsolehandler.h"
#include "util/list.h"

//------------------------------------------------------------------------------
namespace Debug
{
class RenderConsoleHandler : public IO::HistoryConsoleHandler
{
    __DeclareClass(RenderConsoleHandler);
public:
    /// constructor
    RenderConsoleHandler();
    
    /// called by console to output data
    virtual void Print(const Util::String& s);
    /// called by console with serious error
    virtual void Error(const Util::String& s);
    /// called by console to output warning
    virtual void Warning(const Util::String& s);
    /// called by console to output debug string
    virtual void DebugOut(const Util::String& s);
    /// called by Console::Update()
    virtual void Update();

private:
    SizeT linesVisible;
    IndexT curIndex;
};

}; // namespace Render
//------------------------------------------------------------------------------
