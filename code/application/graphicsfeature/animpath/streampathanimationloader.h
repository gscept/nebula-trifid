#pragma once
//------------------------------------------------------------------------------
/**
	@class GraphicsFeature::AnimPathStreamLoader
	
	Used to load animation path resources.
	
	(C) 2015-2016 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------
#include "resources/streamresourceloader.h"
namespace GraphicsFeature
{
class StreamPathAnimationLoader : public Resources::StreamResourceLoader
{
	__DeclareClass(StreamPathAnimationLoader);

private:
	/// setup resource from stream
	bool SetupResourceFromStream(const Ptr<IO::Stream>& stream);
};
} // namespace GraphicsFeature