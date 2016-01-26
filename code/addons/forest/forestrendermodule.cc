//------------------------------------------------------------------------------
// posteffectrendermodule.cc
// (C) 2009 Radon Labs GmbH
//  (C) 2013-2015 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------

#include "stdneb.h"
#include "forestrendermodule.h"
#include "rt/forestrtplugin.h"
#include "graphics/graphicsinterface.h"
#include "graphics/graphicsprotocol.h"
#include "forest/forestprotocol.h"
#include "forest/forestrendermodule.h"

namespace Forest
{
__ImplementClass(ForestRenderModule,'FORM',RenderModules::RenderModule);
__ImplementSingleton(ForestRenderModule);

using namespace Graphics;
using namespace Messaging;
using namespace Util;
using namespace Math;

//------------------------------------------------------------------------------
/**
    Constructor
*/
ForestRenderModule::ForestRenderModule()
{
}

//------------------------------------------------------------------------------
/**
    Destructor	
*/
ForestRenderModule::~ForestRenderModule()
{
}

//------------------------------------------------------------------------------
/**
    Setup the render module	
*/
void
ForestRenderModule::Setup()
{
    n_assert(!this->IsValid());
    RenderModule::Setup();

    // Register the posteffect render-thread plugin
    Ptr<RegisterRTPlugin> msg = RegisterRTPlugin::Create();
    msg->SetType(&ForestRTPlugin::RTTI);
    GraphicsInterface::Instance()->Send(msg.cast<Message>());    
}

//------------------------------------------------------------------------------
/**
    Discard the render module	
*/
void
ForestRenderModule::Discard()
{
    n_assert(this->IsValid());
    RenderModule::Discard();

    // Unregister the forest render-thread plugin
    Ptr<UnregisterRTPlugin> msg = UnregisterRTPlugin::Create();
    msg->SetType(&ForestRTPlugin::RTTI);
    GraphicsInterface::Instance()->Send(msg.cast<Message>());
}

//------------------------------------------------------------------------------
/**
    Set the bounding box of the level to compute the depth of the quadtree
*/
void
ForestRenderModule::LoadLevel(const Math::bbox& boundingBox)
{
    n_assert(this->IsValid());

    // send a load level message to the forest render-thread plugin
    Ptr<Forest::LoadLevel> msg = Forest::LoadLevel::Create();
    msg->SetBoundingBox(boundingBox);
    GraphicsInterface::Instance()->Send(msg.cast<Message>());
}

//------------------------------------------------------------------------------
/**
    Reset the tree server, cleanup all trees and removes the quadtree
*/
void
ForestRenderModule::ExitLevel()
{
    n_assert(this->IsValid());

    // send a exit level message to the forest render-thread plugin
    Ptr<Forest::ExitLevel> msg = Forest::ExitLevel::Create();
    GraphicsInterface::Instance()->Send(msg.cast<Message>());
}

//------------------------------------------------------------------------------
/**
    Create a tree instance and add it to a treecluster
*/
void
ForestRenderModule::CreateTreeInstance(const StringAtom& resourceId, const StringAtom& id, const matrix44& transform, const StringAtom& collideFile)
{
    n_assert(this->IsValid());

    // send a create tree instance message to the forest render-thread plugin
    Ptr<Forest::CreateTreeInstance> msg = Forest::CreateTreeInstance::Create();
    msg->SetResourceId(resourceId);
    msg->SetTreeId(id);
    msg->SetTransform(transform);
    msg->SetCollideFile(collideFile);
    GraphicsInterface::Instance()->Send(msg.cast<Message>());
}

//------------------------------------------------------------------------------
/**
    Setup the tree collides. Call this method once after trees have been
    loaded into the level. All collide geometry will be merged into
    a single collide mesh.
*/
void
ForestRenderModule::SetupTreeCollide()
{
    n_assert(this->IsValid());

    // send a exit level message to the forest render-thread plugin
    Ptr<Forest::SetupCollideMesh> msg = Forest::SetupCollideMesh::Create();
    GraphicsInterface::Instance()->Send(msg.cast<Message>());
}

}