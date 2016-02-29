//------------------------------------------------------------------------------
//  shadowserver.cc
//  (C) 2007 Radon Labs GmbH
//  (C) 2013-2015 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "lighting/shadowserver.h"

namespace Lighting
{
#if (__DX11__ || __OGL4__ || __VULKAN__)
__ImplementClass(Lighting::ShadowServer, 'SDSV', Lighting::SM50ShadowServer);
#elif __DX9__
__ImplementClass(Lighting::ShadowServer, 'SDSV', Lighting::SM30ShadowServer);
#elif __WII__
__ImplementClass(Lighting::ShadowServer, 'SDSV', Wii::WiiShadowServer);
#elif __PS3__
__ImplementClass(Lighting::ShadowServer, 'SDSV', Lighting::ShadowServerBase);
#else
#error "ShadowServer class not implemented on this platform!"
#endif
__ImplementSingleton(Lighting::ShadowServer);

//------------------------------------------------------------------------------
/**
*/
ShadowServer::ShadowServer()
{
    __ConstructSingleton;
}

//------------------------------------------------------------------------------
/**
*/
ShadowServer::~ShadowServer()
{
    __DestructSingleton;
}

} // namespace Lighting
