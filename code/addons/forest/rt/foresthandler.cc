//------------------------------------------------------------------------------
//  foresthandler.cc
//  (C) 2008 Radon Labs GmbH
//  (C) 2013-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "forest/rt/foresthandler.h"
#include "io/memorystream.h"
#include "graphics/graphicsserver.h"
#include "forest/rt/forestserver.h"

namespace Forest
{
__ImplementClass(Forest::ForestHandler, 'FOEH', Interface::InterfaceHandlerBase);

using namespace Messaging;

//------------------------------------------------------------------------------
/**
*/
ForestHandler::ForestHandler()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
ForestHandler::~ForestHandler()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
bool
ForestHandler::HandleMessage(const Ptr<Message>& msg)
{
    n_assert(msg.isvalid());
    if (msg->CheckId(Forest::LoadLevel::Id))
    {
        this->OnLoadLevel(msg.cast<Forest::LoadLevel>());
    }
    else if (msg->CheckId(Forest::ExitLevel::Id))
    {
        this->OnExitLevel();
    }
    else if (msg->CheckId(Forest::CreateTreeInstance::Id))
    {
        this->OnCreateTreeInstance(msg.cast<Forest::CreateTreeInstance>());
    }
    else if (msg->CheckId(Forest::SetupCollideMesh::Id))
    {
        this->OnSetupCollideMesh();
    }
    else
    {
        // unknown message
        return false;
    }
    // fallthrough: message was handled
    return true;
}

//------------------------------------------------------------------------------
/**
*/
void
ForestHandler::OnLoadLevel(const Ptr<Forest::LoadLevel>& msg)
{
    // prepare level and create quadtree
    ForestServer::Instance()->LoadLevel(msg->GetBoundingBox());
}

//------------------------------------------------------------------------------
/**
*/
void
ForestHandler::OnExitLevel()
{
    // reset stuff
    ForestServer::Instance()->ExitLevel();
}

//------------------------------------------------------------------------------
/**
*/
void
ForestHandler::OnCreateTreeInstance(const Ptr<Forest::CreateTreeInstance>& msg)
{
    // add a tree instance to the level
    ForestServer::Instance()->CreateTreeInstance( msg->GetResourceId(),
                                                  msg->GetTreeId(),
                                                  msg->GetTransform(),
                                                  msg->GetCollideFile());
}

//------------------------------------------------------------------------------
/**
*/
void
ForestHandler::OnSetupCollideMesh()
{
    // reset stuff
    ForestServer::Instance()->SetupCollideMesh();
}

} // namespace Debug
