//------------------------------------------------------------------------------
//  animeventemitter.cc
//  (C) 2009 Radon Labs GmbH
//  (C) 2013-2015 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "coreanimation/animeventemitter.h"

namespace CoreAnimation
{
//------------------------------------------------------------------------------
/**
    collects all animevents from given clip
*/
Util::Array<AnimEvent> 
CoreAnimation::AnimEventEmitter::EmitAnimEvents(const AnimClip& clip, Timing::Tick start, Timing::Tick end, float timeFactorInv, bool isInfinite)
{
    Util::Array<AnimEvent> events;

    if (start <= end)
    {
        IndexT startIdx = 0;
		SizeT numEvents = clip.GetEventsInRange(start, end, timeFactorInv, startIdx);
        IndexT i;
        IndexT endIndex = startIdx + numEvents;
        for (i = startIdx; startIdx != InvalidIndex && i < endIndex; ++i)
        {
    	    events.Append(clip.GetEventByIndex(i));
        }
    }
    else
    {
        // from startCheckTime till end of clip
        Timing::Tick clipDuration = clip.GetClipDuration();
		events.AppendArray(AnimEventEmitter::EmitAnimEvents(clip, start, clipDuration, timeFactorInv, false));
        if (isInfinite)
        {
            // from start of clip till endCheckTime
			events.AppendArray(AnimEventEmitter::EmitAnimEvents(clip, 0, end, timeFactorInv, false));
        }
    }
    return events;
}
}