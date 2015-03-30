#pragma once
//------------------------------------------------------------------------------
/**
    @class Characters::SkinnedMeshDrawInfo
  
    Front-end class for software-skinned draw info struct.
    
    (C) 2009 Radon Labs GmbH
    (C) 2013-2015 Individual contributors, see AUTHORS file
*/
#if __WII__
#include "characters/wii/wiiskinnedmeshdrawinfo.h"
namespace Characters
{
struct SkinnedMeshDrawInfo : public Wii::WiiSkinnedMeshDrawInfo
{ };
}
#elif __PS3__
#include "characters/ps3/ps3skinnedmeshdrawinfo.h"
namespace Characters
{
struct SkinnedMeshDrawInfo : public PS3::PS3SkinnedMeshDrawInfo
{ };
}
#elif (__WIN32__ || __XBOX360__ || __LINUX__ )
#include "characters/base/skinnedmeshdrawinfobase.h"
namespace Characters
{
struct SkinnedMeshDrawInfo : public Base::SkinnedMeshDrawInfoBase
{ };
}
#else
#error "Characters::SkinnedMeshDrawInfo not implemented on this platform!"
#endif
