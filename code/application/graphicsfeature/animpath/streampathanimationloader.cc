//------------------------------------------------------------------------------
//  animpathstreamloader.cc
//  (C) 2015-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "streampathanimationloader.h"
#include "io/xmlreader.h"
#include "coreanimation/infinitytype.h"
#include "pathanimation.h"

using namespace IO;
namespace GraphicsFeature
{
__ImplementClass(GraphicsFeature::StreamPathAnimationLoader, 'APSL', Resources::StreamResourceLoader);

//------------------------------------------------------------------------------
/**
*/
bool
StreamPathAnimationLoader::SetupResourceFromStream(const Ptr<IO::Stream>& stream)
{
	const Ptr<PathAnimation> animPathRes = this->resource.downcast<PathAnimation>();
	Ptr<XmlReader> xmlReader = XmlReader::Create();
	xmlReader->SetStream(stream);
	if (xmlReader->Open())
	{
		if (xmlReader->HasNode("/NebulaT"))
		{
			xmlReader->SetToNode("/NebulaT");
			if (xmlReader->SetToFirstChild("Track")) do
			{
				Ptr<PathAnimationTrack> track = PathAnimationTrack::Create();
				Util::String name = xmlReader->GetString("name");

				// read looping attribute for rotation
				if (xmlReader->HasAttr("rotationInfinity"))
				{
					Util::String infinity = xmlReader->GetString("rotationInfinity");
					CoreAnimation::InfinityType::Code code = CoreAnimation::InfinityType::FromString(infinity);
					track->rotation.SetInfinity(code);
				}

				// read looping attribute for translation
				if (xmlReader->HasAttr("translationInfinity"))
				{
					Util::String infinity = xmlReader->GetString("translationInfinity");
					CoreAnimation::InfinityType::Code code = CoreAnimation::InfinityType::FromString(infinity);
					track->translation.SetInfinity(code);
				}

				// read looping attribute for scaling
				if (xmlReader->HasAttr("scalingInfinity"))
				{
					Util::String infinity = xmlReader->GetString("scalingInfinity");
					CoreAnimation::InfinityType::Code code = CoreAnimation::InfinityType::FromString(infinity);
					track->scaling.SetInfinity(code);
				}
				
				// setup default start values				
				track->scaling.Begin(Math::float4(1));
				track->translation.Begin(Math::float4(0));
				track->rotation.Begin(Math::float4(0));
				track->name = name;

				// get translation
				if (xmlReader->SetToFirstChild("Translate")) do
				{
					bool hasC0 = xmlReader->HasAttr("c0");
					bool hasC1 = xmlReader->HasAttr("c1");
					if (hasC0 && hasC1)
					{
						track->translation.CubicTo(
							xmlReader->GetFloat4("to"),
							xmlReader->GetFloat4("c0"),
							xmlReader->GetFloat4("c1"),
							xmlReader->GetFloat("start"),
							xmlReader->GetFloat("end"));
					}
					else if (hasC0)
					{
						track->translation.QuadraticTo(
							xmlReader->GetFloat4("to"),
							xmlReader->GetFloat4("c0"),
							xmlReader->GetFloat("start"),
							xmlReader->GetFloat("end"));
					}
					else
					{
						track->translation.LinearTo(
							xmlReader->GetFloat4("to"),
							xmlReader->GetFloat("start"),
							xmlReader->GetFloat("end"));
					}
				}
				while (xmlReader->SetToNextChild("Translate"));

				// get rotation
				if (xmlReader->SetToFirstChild("Rotate")) do
				{
					bool hasC0 = xmlReader->HasAttr("c0");
					bool hasC1 = xmlReader->HasAttr("c1");
					if (hasC0 && hasC1)
					{
						track->rotation.CubicTo(
							xmlReader->GetFloat4("to"),
							xmlReader->GetFloat4("c0"),
							xmlReader->GetFloat4("c1"),
							xmlReader->GetFloat("start"),
							xmlReader->GetFloat("end"));
					}
					else if (hasC0)
					{
						track->rotation.QuadraticTo(
							xmlReader->GetFloat4("to"),
							xmlReader->GetFloat4("c0"),
							xmlReader->GetFloat("start"),
							xmlReader->GetFloat("end"));
					}
					else
					{
						track->rotation.LinearTo(
							xmlReader->GetFloat4("to"),
							xmlReader->GetFloat("start"),
							xmlReader->GetFloat("end"));
						
					}
				}
				while (xmlReader->SetToNextChild("Rotate"));

				// get rotation
				if (xmlReader->SetToFirstChild("Scale")) do
				{
					bool hasC0 = xmlReader->HasAttr("c0");
					bool hasC1 = xmlReader->HasAttr("c1");
					if (hasC0 && hasC1)
					{
						track->scaling.CubicTo(
							xmlReader->GetFloat4("to"),
							xmlReader->GetFloat4("c0"),
							xmlReader->GetFloat4("c1"),
							xmlReader->GetFloat("start"),
							xmlReader->GetFloat("end"));
					}
					else if (hasC0)
					{
						track->scaling.QuadraticTo(
							xmlReader->GetFloat4("to"),
							xmlReader->GetFloat4("c0"),
							xmlReader->GetFloat("start"),
							xmlReader->GetFloat("end"));
					}
					else
					{
						track->scaling.LinearTo(
							xmlReader->GetFloat4("to"),
							xmlReader->GetFloat("start"),
							xmlReader->GetFloat("end"));
					}
				}
				while (xmlReader->SetToNextChild("Scale"));

				// add track to resource
				animPathRes->tracks.Add(name, track);

				// get all events
				if (xmlReader->SetToFirstChild("Event")) do
				{
					Util::String name = xmlReader->GetString("name");
					float time = xmlReader->GetFloat("time");
					track->events.Add(time, name);
				}
				while (xmlReader->SetToNextChild("Event"));
			}
			while (xmlReader->SetToNextChild("Track"));
		}
		else
		{
			// doesn't contain NebulaT main node
			return false;
		}
	}
	else
	{
		// not a valid XML
		return false;
	}

	// fallthrough is that everything works fine
	return true;
}

} // namespace GraphicsFeature