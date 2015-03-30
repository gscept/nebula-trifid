#pragma once
//------------------------------------------------------------------------------
/**
    @class FAudio::EventState

    information about the state of an fmod-event
    
    (C) 2015 Individual contributors, see AUTHORS file
*/

//------------------------------------------------------------------------------
namespace FAudio
{
    enum EventState
    {
        EventStateUnknown = 0,
        EventPlaying = 1 << 0,
        EventPaused  = 1 << 1,
        EventStopped = 1 << 2
    };
} // namespace FAudio
//------------------------------------------------------------------------------
