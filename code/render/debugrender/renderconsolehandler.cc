//------------------------------------------------------------------------------
//  renderconsolehandler.cc
//  (C) 2006 Radon Labs GmbH
//  (C) 2013-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "debugrender/renderconsolehandler.h"
#include "core/coreserver.h"
#include "core/sysfunc.h"
#include "threading/thread.h"
#include "debugrender/debugrender.h"

#if !__WII__
#include "input/keyboard.h"
#include "input/inputserver.h"
#endif

namespace Debug
{
__ImplementClass(Debug::RenderConsoleHandler, 'DREC', IO::HistoryConsoleHandler);

using namespace Util;
using namespace Math;

//------------------------------------------------------------------------------
/**
*/
RenderConsoleHandler::RenderConsoleHandler():
    linesVisible(12),
    curIndex(0)
{    
}

//------------------------------------------------------------------------------
/**
*/
void
RenderConsoleHandler::Print(const String& s)
{
    HistoryConsoleHandler::Print(s);    
    this->curIndex = n_max(0,(int)this->GetHistory().Size() - (int)this->linesVisible);
}

//------------------------------------------------------------------------------
/**
*/
void
RenderConsoleHandler::DebugOut(const String& s)
{
    HistoryConsoleHandler::DebugOut(s);
    this->curIndex = n_max(0,(int)this->GetHistory().Size() - (int)this->linesVisible);
}

//------------------------------------------------------------------------------
/**
*/
void
RenderConsoleHandler::Error(const String& msg)
{
    const char* appName = "???";
    if (Core::CoreServer::HasInstance())
    {
        appName = Core::CoreServer::Instance()->GetAppName().Value();
    }
    String str;
    str.Format("*** ERROR ***\nApplication: %s\nError: %s", appName, msg.AsCharPtr());
    
    HistoryConsoleHandler::Error(str);
    Core::SysFunc::Error(str.AsCharPtr());
    this->curIndex = n_max(0,(int)this->GetHistory().Size() - (int)this->linesVisible);
}

//------------------------------------------------------------------------------
/**
*/
void
RenderConsoleHandler::Warning(const String& s)
{
    HistoryConsoleHandler::Warning(s);
    this->curIndex = n_max(0,(int)this->GetHistory().Size() - (int)this->linesVisible);
}

//------------------------------------------------------------------------------
/**
*/
void 
RenderConsoleHandler::Update()
{
    #if !__WII__  
    if (Input::InputServer::HasInstance())
    {
        // get ring buffer
        const RingBuffer<Util::String> & history = this->GetHistory();
        
        // check keyboard up and down keys  
        Ptr<Input::InputServer> inputServer = Input::InputServer::Instance();
        Ptr<Input::Keyboard> keyboard = inputServer->GetDefaultKeyboard();
        if(keyboard->KeyUp(Input::Key::Down))
        {
            this->curIndex++;
            this->curIndex = n_max(0,n_min((int)history.Size() - (int)this->linesVisible, this->curIndex));
        }
        else if(keyboard->KeyUp(Input::Key::Up))
        {
            this->curIndex--;
            this->curIndex = n_max(0, this->curIndex);
        }
        
        // print all lines on screen
        const Math::float4 color(1,1,1,1);
        Math::float2 pos(0,0);
        
        for(IndexT i = this->curIndex; i < this->curIndex + this->linesVisible && i < history.Size(); i++)
        {
            _debug_text(history[i], pos, color);
            Array<String> numlineBreaks = history[i].Tokenize("\n");
            pos.y() += (numlineBreaks.Size()) * 0.025f;
        }
               
    }
    #endif
}

} // namespace Debug