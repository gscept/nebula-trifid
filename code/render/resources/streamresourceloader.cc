//------------------------------------------------------------------------------
//  streamresourceloader.cc
//  (C) 2008 Radon Labs GmbH
//  (C) 2013-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "resources/streamresourceloader.h"
#include "io/memorystream.h"
#include "io/iointerface.h"

namespace Resources
{
__ImplementClass(Resources::StreamResourceLoader, 'SRLD', Resources::ResourceLoader);

using namespace IO;
using namespace Messaging;

//------------------------------------------------------------------------------
/**
*/
StreamResourceLoader::StreamResourceLoader()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
StreamResourceLoader::~StreamResourceLoader()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    Indicate whether this resource loader supports asynchronous loading.
    The default is true. Override this method in a subclass and return
    false otherwise.
*/
bool
StreamResourceLoader::CanLoadAsync() const
{
    return true;
}

//------------------------------------------------------------------------------
/**
    Handle the generic load request. In the asynchronous case, this method
    will fire a ReadStream message and go into pending state. The client
    will then call OnPending() periodically which checks whether the 
    ReadStream message has been handled and continue accordingly. In the
    synchronous state the method will create an IO::Stream object and
    call the SetupResourceFromStream() method directly.
*/
bool
StreamResourceLoader::OnLoadRequested()
{
    n_assert(this->GetState() == Resource::Initial);
    n_assert(this->resource.isvalid());
    if (this->resource->IsAsyncEnabled())
    {
        // perform asynchronous load
        n_assert(!this->readStreamMsg.isvalid());
        this->readStreamMsg = ReadStream::Create();
        this->readStreamMsg->SetURI(this->resource->GetResourceId().Value());
        this->readStreamMsg->SetStream(MemoryStream::Create());
        IoInterface::Instance()->Send(this->readStreamMsg.upcast<Message>());
        
        // go into Pending state
        this->SetState(Resource::Pending);
        return true;
    }
    else
    {
        // perform synchronous load
        Ptr<Stream> stream = IoServer::Instance()->CreateStream(this->resource->GetResourceId().Value());
        if (this->SetupResourceFromStream(stream))
        {
            this->SetState(Resource::Loaded);
            return true;
        }

        // fallthrough: synchronous loading failed
        this->SetState(Resource::Failed);
        return false;
    }
}

//------------------------------------------------------------------------------
/**
    This method is called when the currently pending asynchronous load request 
    should be cancelled.
*/
void
StreamResourceLoader::OnLoadCancelled()
{
    n_assert(this->GetState() == Resource::Pending);
    n_assert(this->readStreamMsg.isvalid());
    IoInterface::Instance()->Cancel(this->readStreamMsg.upcast<Message>());
    this->readStreamMsg = 0;
    ResourceLoader::OnLoadCancelled();
}

//------------------------------------------------------------------------------
/**
    This method is called periodically by the client when the 
    resource is in pending state. The pending ReadStream message
    will be checked, and if it has been handled successfully the 
    SetupResourceFromStream() method will be called and the Resource
    will go into Loaded state. If anything goes wrong, the resource
    will go into Failed state.
*/
bool
StreamResourceLoader::OnPending()
{
    n_assert(this->GetState() == Resource::Pending);
    n_assert(this->readStreamMsg.isvalid());
    bool retval = false;

    // check if asynchronous loader job has finished
    if (this->readStreamMsg->Handled())
    {
        // ok, loader job has finished
        if (this->readStreamMsg->GetResult())
        {
            // IO operation was successful
            if (this->SetupResourceFromStream(this->readStreamMsg->GetStream()))
            {
                // everything ok!
                this->SetState(Resource::Loaded);                
                retval = true;
            }
            else
            {
                // file not found or not a valid file...
                this->SetState(Resource::Failed);
            }
        }
        else
        {
            // error during IO operation
            this->SetState(Resource::Failed);
        }
        // we no longer need the loader job message
        this->readStreamMsg = 0;
    }
    return retval;
}

//------------------------------------------------------------------------------
/**
    The SetupResourceFromStream should be overwritten by the subclass and
    is called when the stream containing the resource data is available
    (right from OnLoadRequested() in the synchronous state, or at some
    later time from OnPending() in the asynchronous state. Subclasses should
    setup the resource object of this loader from the stream content and
    return true on success, or false on failure.
*/
bool
StreamResourceLoader::SetupResourceFromStream(const Ptr<Stream>& stream)
{
    // override in subclass!
    n_error("Pure virtual method called: StreamResourceLoader::SetupResourceFromStream()!");
    return false;
}

} // namespace Resources