//------------------------------------------------------------------------------
//  animpathresource.cc
//  (C) 2015-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "pathanimation.h"
#include "animpath.h"

namespace GraphicsFeature
{
__ImplementClass(GraphicsFeature::PathAnimation, 'APRS', Resources::Resource);
__ImplementClass(GraphicsFeature::PathAnimationTrack, 'PATR', Core::RefCounted);

//------------------------------------------------------------------------------
/**
*/
PathAnimation::PathAnimation()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
PathAnimation::~PathAnimation()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
void
PathAnimation::Unload()
{
	IndexT i;
	for (i = 0; i < this->tracks.Size(); i++)
	{
		this->tracks.ValueAtIndex(i)->rotation.Cleanup();
		this->tracks.ValueAtIndex(i)->translation.Cleanup();
		this->tracks.ValueAtIndex(i)->scaling.Cleanup();
	}
	this->tracks.Clear();
	Resources::Resource::Unload();
}

//------------------------------------------------------------------------------
/**
*/
const Util::Array<Util::String>
PathAnimationTrack::GetEvents(const float start, const float end)
{
	Util::Array<Util::String> events;
	IndexT i;
	for (i = 0; i < this->events.Size(); i++)
	{
		const Util::KeyValuePair<float, Util::String>& kvp = this->events.KeyValuePairAtIndex(i);
		if (kvp.Key() >= start)
		{
			// if the key value is bigger than our end interval, 
			if (kvp.Key() > end) break;
			else
			{
				events.Append(kvp.Value());
			}
		}
	}

	return events;
}

} // namespace GraphicsFeature