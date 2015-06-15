#pragma once
//------------------------------------------------------------------------------
/**
	@class Materials::StreamSurfaceMaterialSaver
	
	Saves a surface material to an XML file.
	
	(C) 2015 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------
#include "resources/resourcesaver.h"
#include "io/stream.h"
#include "io/iointerfaceprotocol.h"
namespace Materials
{
class StreamSurfaceMaterialSaver : public Resources::ResourceSaver
{
	__DeclareClass(StreamSurfaceMaterialSaver);
public:
	/// constructor
	StreamSurfaceMaterialSaver();
	/// destructor
	virtual ~StreamSurfaceMaterialSaver();

	/// set stream to save to
	void SetStream(const Ptr<IO::Stream>& stream);
	/// get save-stream
	const Ptr<IO::Stream>& GetStream() const;

	/// called by resource when a save is requested
	bool OnSave();

private:
	Ptr<IO::Stream> stream;
};

//------------------------------------------------------------------------------
/**
*/
inline void
StreamSurfaceMaterialSaver::SetStream(const Ptr<IO::Stream>& s)
{
	this->stream = s;
}

//------------------------------------------------------------------------------
/**
*/
inline const Ptr<IO::Stream>&
StreamSurfaceMaterialSaver::GetStream() const
{
	return this->stream;
}

} // namespace Materials