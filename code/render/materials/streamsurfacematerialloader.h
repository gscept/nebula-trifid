#pragma once
//------------------------------------------------------------------------------
/**
	@class Materials::StreamSurfaceMaterialLoader
	
	Loads surface materials from XML declaration.
	
	(C) 2015 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------
#include "resources/resourceloader.h"
#include "io/stream.h"
#include "io/iointerfaceprotocol.h"
namespace Materials
{
class StreamSurfaceMaterialLoader : public Resources::ResourceLoader
{
	__DeclareClass(StreamSurfaceMaterialLoader);
public:
	/// constructor
	StreamSurfaceMaterialLoader();
	/// destructor
	virtual ~StreamSurfaceMaterialLoader();

    /// return true if asynchronous loading is supported
    virtual bool CanLoadAsync() const;
    /// called by resource when a load is requested
    virtual bool OnLoadRequested();
    /// called by resource to cancel a pending load
    virtual void OnLoadCancelled();
    /// call frequently while after OnLoadRequested() to put Resource into loaded state
    virtual bool OnPending();

private:
    /// setup material from stream
    bool SetupMaterialFromStream(const Ptr<IO::Stream>& stream);

    Ptr<IO::ReadStream> readStreamMsg;
};
} // namespace Materials