#pragma once
//------------------------------------------------------------------------------
/**
	@class Physics::VisualDebuggerServer

    Server for the physics-engine visualdebugger program. Holds convenience
	methods to easily display debug-data via the debugger

    (C) 2013 JM
*/

#if (__USE_BULLET__)
#include "base/basevisualdebuggerserver.h"
namespace Physics
{
class VisualDebuggerServer : public Physics::BaseVisualDebuggerServer
{
	__DeclareClass(VisualDebuggerServer);   
	__DeclareInterfaceSingleton(VisualDebuggerServer);  
public:
	/// constructor
	VisualDebuggerServer();
	/// destructor
	virtual ~VisualDebuggerServer();
};
}
#elif(__USE_PHYSX__)
#include "base/basevisualdebuggerserver.h"
namespace Physics
{
class VisualDebuggerServer : public Physics::BaseVisualDebuggerServer
{
	__DeclareClass(VisualDebuggerServer);   
	__DeclareInterfaceSingleton(VisualDebuggerServer);      
public:
	/// constructor
	VisualDebuggerServer();
	/// destructor
	virtual ~VisualDebuggerServer();
};
}
#elif(__USE_HAVOK__)
#include "havok/havokvisualdebuggerserver.h"
namespace Physics
{
class VisualDebuggerServer : public Havok::HavokVisualDebuggerServer
{
	__DeclareClass(VisualDebuggerServer);   
	__DeclareInterfaceSingleton(VisualDebuggerServer);   
public:
	/// constructor
	VisualDebuggerServer();
	/// destructor
	virtual ~VisualDebuggerServer();
};
}
#else
#error "Physics::VisualDebuggerServer not implemented"
#endif