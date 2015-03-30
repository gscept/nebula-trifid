#pragma once
//#include "core/refcounted.h"
#include <QtCore/qobject.h>
namespace Core
{
    class RefCounted;
}
//------------------------------------------------------------------------------
/**
    @file config.h
    
    Contains utility functions and defines for Nody.
    
    (C) 2013 Gustav Sterbrant
*/
//------------------------------------------------------------------------------

// we define a set of layers at which we should render objects
#define NODESUBLAYER 1		// this is the sub-layer for nodes, all nodes should be here except for the 'focused' node
#define NODESUPERLAYER 2	// this is where the focused node should be
#define LINKLAYER INT_MAX   // links should lie on top of everything else
#define TRASHLAYER 4

// we define a location at which we store a user-pointer to a Nebula3-style object, this should be used for setData
#define NEBULAUSERPOINTERLOCATION 0
#define NEBULAUSERPROPERTYNAME "NEBULA"

//------------------------------------------------------------------------------------
/**
	Converts an unsigned pointer into a defined type.
	Useful for storing pointers in Qt's QVariant class.
    Returns true if conversion is successful, the second argument is a pointer to the type expected.

	ONLY USE WITH NEBULA OBJECTS!
*/
template<class CLASS>
bool
n_convertinttotype(unsigned ptr, Core::RefCounted** obj)
{
    QObject* qobj = reinterpret_cast<QObject*>(ptr);
	CLASS* baseObj = dynamic_cast<CLASS*>(qobj);
    if (baseObj != 0)
    {
        *obj = (Core::RefCounted*)baseObj;
        return true;
    }
    else
    {
        *obj = 0;
        return false;
    }
}

template<class CLASS>
bool n_convertvoidtotype(void* ptr, Core::RefCounted** obj)
{
    CLASS* baseObj = reinterpret_cast<CLASS*>(ptr);
    if (baseObj != 0)
    {
        *obj = (Core::RefCounted*)baseObj;
        return true;
    }
    else
    {
        *obj = 0;
        return false;
    }
}
	

// VisualStudio settings
#ifdef _MSC_VER
#define __VC__ (1)
#endif
#ifdef __VC__
#pragma warning( disable : 4251 )       // class XX needs DLL interface to be used...
#pragma warning( disable : 4355 )       // initialization list uses 'this' 
#pragma warning( disable : 4275 )       // base class has not dll interface...
#pragma warning( disable : 4786 )       // symbol truncated to 255 characters
#pragma warning( disable : 4530 )       // C++ exception handler used, but unwind semantics not enabled
#pragma warning( disable : 4995 )       // _OLD_IOSTREAMS_ARE_DEPRECATED
#pragma warning( disable : 4996 )       // _CRT_INSECURE_DEPRECATE, VS8: old string routines are deprecated
#pragma warning( disable : 4512 )       // 'class' : assignment operator could not be generated
#pragma warning( disable : 4610 )       // object 'class' can never be instantiated - user-defined constructor required
#pragma warning( disable : 4510 )       // 'class' : default constructor could not be generated
#ifdef __WIN64__
#pragma warning( disable : 4267 )
#endif
#endif

// GCC settings
#if defined __GNUC__
#define __cdecl
#define __forceinline inline __attribute__((always_inline))
#pragma GCC diagnostic ignored "-Wmultichar"
#pragma GCC diagnostic ignored "-Wformat-security"
#endif
