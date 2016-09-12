//------------------------------------------------------------------------------
// frameserver.cc
// (C) 2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "frameserver.h"
#include "framescriptloader.h"

namespace Frame2
{

__ImplementClass(Frame2::FrameServer, 'FRSR', Core::RefCounted);
__ImplementSingleton(Frame2::FrameServer);
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
	__DestructSingleton;
}

//------------------------------------------------------------------------------
/**
*/
void
FrameServer::Open()
{
	n_assert(!this->isOpen);
	this->isOpen = true;
}

//------------------------------------------------------------------------------
/**
*/
void
FrameServer::Close()
{
	n_assert(this->isOpen);
	/*
	IndexT i;
	for (i = 0; i < this->frameScripts.Size(); i++)
	{
		this->frameScripts.ValueAtIndex(i)->Discard();
	}
	*/
	this->frameScripts.Clear();
}

//------------------------------------------------------------------------------
/**
*/
Ptr<Frame2::FrameScript>
FrameServer::LoadFrameScript(const Resources::ResourceId& name, const IO::URI& path)
{
	n_assert(!this->frameScripts.Contains(name));
	Ptr<Frame2::FrameScript> script = FrameScriptLoader::LoadFrameScript(path);
	this->frameScripts.Add(name, script);
	return script;
}


} // namespace Frame2