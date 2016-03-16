#pragma once
//------------------------------------------------------------------------------
/**
	@class GraphicsFeature::AnimPathResource
	
	The anim path resource is a load once, reusable resource which is declared 
	as a series of animation paths pertaining to translation, rotation and scaling of an object.

	By default, the translation, rotation and scaling attributes are constant, and must be defined as cycling if such behavior is wanted.
	
	(C) 2015-2016 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------
#include "resources/resource.h"
#include "animpath.h"
namespace GraphicsFeature
{

class PathAnimationTrack : public Core::RefCounted
{
	__DeclareClass(PathAnimationTrack);

public:
	/// return events based on time slice
	const Util::Array<Util::String> GetEvents(const float start, const float end);

	AnimPath<Math::float4> translation;
	AnimPath<Math::float4> rotation;
	AnimPath<Math::float4> scaling;

private:
	friend class StreamPathAnimationLoader;
	Util::Dictionary<float, Util::String> events;
	Util::String name;
};

class PathAnimation : public Resources::Resource
{
	__DeclareClass(PathAnimation);
public:

	/// constructor
	PathAnimation();
	/// destructor
	virtual ~PathAnimation();

	/// return track based on name
	const Ptr<PathAnimationTrack>& GetTrack(const Util::String& track);

	/// unload resource
	void Unload();

private:
	friend class StreamPathAnimationLoader;
	Util::Dictionary<Util::String, Ptr<PathAnimationTrack>> tracks;
};


//------------------------------------------------------------------------------
/**
*/
inline const Ptr<PathAnimationTrack>&
PathAnimation::GetTrack(const Util::String& track)
{
	n_assert(this->tracks.Contains(track));
	return this->tracks[track];
}


} // namespace GraphicsFeature