//------------------------------------------------------------------------------
// posteffectrendermodule.cc
// (C) 2009 Radon Labs GmbH
//  (C) 2013-2015 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------

#include "stdneb.h"
#include "posteffectrendermodule.h"
#include "rt/posteffectrtplugin.h"
#include "graphics/graphicsinterface.h"
#include "graphics/graphicsprotocol.h"

namespace PostEffect
{
__ImplementClass(PostEffectRenderModule,'PERM',RenderModules::RenderModule);
__ImplementSingleton(PostEffectRenderModule);

using namespace Graphics;
using namespace Messaging;

//------------------------------------------------------------------------------
/**
    Constructor
*/
PostEffectRenderModule::PostEffectRenderModule()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    Destructor	
*/
PostEffectRenderModule::~PostEffectRenderModule()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    Setup the render module	
*/
void
PostEffectRenderModule::Setup()
{
    n_assert(!this->IsValid());
    RenderModule::Setup();

      
}

//------------------------------------------------------------------------------
/**
    Discard the render module	
*/
void
PostEffectRenderModule::Discard()
{
    n_assert(this->IsValid());
    RenderModule::Discard();

 
}

}