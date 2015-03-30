#ifndef BENCHMARKING_CREATEOBJECTSBYCLASSNAME_H
#define BENCHMARKING_CREATEOBJECTSBYCLASSNAME_H
//------------------------------------------------------------------------------
/**
    @class Benchmarking::CreateObjectsByClassName
  
    Create many objects by their class name.
    
    (C) 2006 Radon Labs GmbH
*/
#include "benchmarkbase/benchmark.h"

//------------------------------------------------------------------------------
namespace Benchmarking
{
class CreateObjectsByClassName : public Benchmark
{
    __DeclareClass(CreateObjectsByClassName);
public:
    /// run the benchmark
    virtual void Run(Timing::Timer& timer);
};

} // namespace Benchmark
//------------------------------------------------------------------------------
#endif
