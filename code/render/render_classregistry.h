#pragma once
//------------------------------------------------------------------------------
/**
    @file render.h
    
    Add all class headers for classes which need dynamic object creation
    by class name or class fourcc here.
    
    (C) 2007 Radon Labs GmbH
*/
#include "models/modelnode.h"
#include "models/nodes/shapenode.h"
#include "characters/characternode.h"
#include "characters/characterskinnode.h"
#include "models/nodes/animatornodeinstance.h"
#include "particles/particlesystemnode.h"
#include "frame/framepass.h"

#if __WII__
#include "frame/wii/wiifsaaframepass.h"
#endif

namespace Models
{
    __RegisterClass(ModelNode);
    __RegisterClass(ShapeNode);
}

namespace Characters
{
    __RegisterClass(CharacterNode);
    __RegisterClass(CharacterSkinNode);
}

namespace Particles
{
    __RegisterClass(ParticleSystemNode);
}

namespace Frame
{
    __RegisterClass(FramePass);
}

#if __WII__
namespace Wii
{
    __RegisterClass(WiiFSAAFramePass);
}
#endif
