//------------------------------------------------------------------------------
//  createobjectsbyclassname.cc
//  (C) 2006 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "createobjectsbyclassname.h"
#include "core/factory.h"

namespace Benchmarking
{
__ImplementClass(Benchmarking::CreateObjectsByClassName, 'BCCN', Benchmarking::Benchmark);

using namespace Core;
using namespace Timing;
using namespace Util;

//------------------------------------------------------------------------------
/**
*/
void
CreateObjectsByClassName::Run(Timer& timer)
{
    // create a million RefCounted objects
    const int numObjects = 1000000;
    Factory* factory = Factory::Instance();
    String className("Core::RefCounted");
    Ptr<RefCounted> ptr;
    int i;
    timer.Start();
    for (i = 0; i < numObjects; i++)
    {
        ptr = factory->Create(className);
    }
    timer.Stop();
}

} // namespace Benchmark
