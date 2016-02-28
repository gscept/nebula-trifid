#pragma once
//------------------------------------------------------------------------------
/**
    @class Lighting::ShadowServer
    
    The ShadowServer setups and controls the global aspects of the dynamic
    shadow system.
    
    (C) 2007 Radon Labs GmbH
    (C) 2013-2016 Individual contributors, see AUTHORS file
*/
#if (__DX11__ || __OGL4__)
#include "lighting/sm50/sm50shadowserver.h"
namespace Lighting
{
class ShadowServer : public SM50ShadowServer
{
	__DeclareClass(ShadowServer);
	__DeclareSingleton(ShadowServer);
public:
	/// constructor
	ShadowServer();
	/// destructor
	virtual ~ShadowServer();
};
} // namespace Lighting
#elif __DX9__
#include "lighting/sm30/sm30shadowserver.h"
namespace Lighting
{
class ShadowServer : public SM30ShadowServer
{
    __DeclareClass(ShadowServer);
    __DeclareSingleton(ShadowServer);
public:
    /// constructor
    ShadowServer();
    /// destructor
    virtual ~ShadowServer();
};
} // namespace Lighting
#elif __WII__
#include "lighting/wii/wiishadowserver.h"
namespace Lighting
{
class ShadowServer : public Wii::WiiShadowServer
{
    __DeclareClass(ShadowServer);
    __DeclareSingleton(ShadowServer);
public:
    /// constructor
    ShadowServer();
    /// destructor
    virtual ~ShadowServer();
};
} // namespace Lighting
#elif __PS3__
#include "lighting/ps3/ps3shadowserver.h"
namespace Lighting
{
class ShadowServer : public PS3::PS3ShadowServer
{
    __DeclareClass(ShadowServer);
    __DeclareSingleton(ShadowServer);
public:
    /// constructor
    ShadowServer();
    /// destructor
    virtual ~ShadowServer();
};
} // namespace Lighting
#else
#error "ShadowServer class not implemented on this platform!"
#endif
//------------------------------------------------------------------------------
