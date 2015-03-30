#ifndef BENCHMARKING_CREATEOBJECTS_H
#define BENCHMARKING_CREATEOBJECTS_H
//------------------------------------------------------------------------------
/**
    @class Benchmarking::CreateObjects
    
    Benchmarks creation (and destruction) of many objects directly through the 
    Create() method and storing the pointers in a smart pointer.
    
    (C) 2006 Radon Labs GmbH
*/
#include "benchmarkbase/benchmark.h"

//------------------------------------------------------------------------------
namespace Benchmarking
{
class CreateObjects : public Benchmark
{
    __DeclareClass(CreateObjects);
public:
    /// run the benchmark
    virtual void Run(Timing::Timer& timer);
};

} // namespace Benchmark
//------------------------------------------------------------------------------
#endif
