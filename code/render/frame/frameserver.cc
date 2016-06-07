//------------------------------------------------------------------------------
//  frameserver.cc
//  (C) 2007 Radon Labs GmbH
//  (C) 2013-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "frame/frameserver.h"
#include "io/ioserver.h"
#include "io/uri.h"
#include "frame/frameshaderloader.h"

namespace Frame
{
__ImplementClass(Frame::FrameServer, 'FSRV', Core::RefCounted);
__ImplementSingleton(Frame::FrameServer);

using namespace Util;
using namespace IO;
using namespace Resources;

//------------------------------------------------------------------------------
/**
*/
FrameServer::FrameServer() :
    isOpen(false)
{
    __ConstructSingleton;
}        

//------------------------------------------------------------------------------
/**
*/
FrameServer::~FrameServer()
{
    if (this->IsOpen())
    {
        this->Close();
    }
    __DestructSingleton;
}

//------------------------------------------------------------------------------
/**
*/
bool
FrameServer::Open()
{
    n_assert(!this->IsOpen());
    this->isOpen = true;
    return true;
}

//------------------------------------------------------------------------------
/**
*/
void
FrameServer::Close()
{
    n_assert(this->IsOpen());

    // discard frame shaders
    IndexT i;
    for (i = 0; i < this->frameShaders.Size(); i++)
    {
        this->frameShaders.ValueAtIndex(i)->Discard();
    }
    this->frameShaders.Clear();
    this->isOpen = false;
}

//------------------------------------------------------------------------------
/**
*/
void
FrameServer::LoadFrameShader(const ResourceId& name)
{
    n_assert(!this->frameShaders.Contains(name));

    String path("frame:");
    path.Append(name.AsString());
    path.Append(".xml");
    Ptr<FrameShader> frameShader = FrameShaderLoader::LoadFrameShader(name, path);
    this->frameShaders.Add(name, frameShader);
}

//------------------------------------------------------------------------------
/**
*/
Ptr<FrameShader>
FrameServer::LookupFrameShader(const ResourceId& resId)
{
    if (!this->frameShaders.Contains(resId))
    {
        this->LoadFrameShader(resId);
    }
    return this->frameShaders[resId];
}

//------------------------------------------------------------------------------
/**
*/
void
FrameServer::DiscardFrameShader(const Ptr<FrameShader>& frameShader)
{
	n_assert(this->frameShaders.Contains(frameShader->GetName()));
	this->frameShaders.Erase(frameShader->GetName());
}

//------------------------------------------------------------------------------
/**
	Perform actual resize
*/
void 
FrameServer::DisplayResized(SizeT width, SizeT height)
{
	// refresh frameshaders
	IndexT i;
	for (i = 0; i < this->frameShaders.Size(); i++)
	{
		this->frameShaders.ValueAtIndex(i)->OnWindowResized(width, height);
	}
}

} // namespace Frame
