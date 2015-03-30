#pragma once
//------------------------------------------------------------------------------
/**
    @class Core::RefCountedList
  
    Implements a static list which keeps track of all refcounted objects
    to detect refcounting leaks at application shutdown. Will only
    be active when the application is compiled in debug mode.
    
    (C) 2006 Radon Labs GmbH
    (C) 2013-2015 Individual contributors, see AUTHORS file
*/
#include "core/types.h"
#include "util/list.h"

//------------------------------------------------------------------------------
namespace Core
{
class RefCounted;

class RefCountedList : public Util::List<RefCounted*>
{
public:
    /// dump memory leaks, this methods is called by RefCounted::DumpRefCountedLeaks()
    void DumpLeaks();
};

}
//------------------------------------------------------------------------------
