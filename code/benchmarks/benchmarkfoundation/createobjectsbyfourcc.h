#ifndef BENCHMARKING_CREATEOBJECTSBYFOURCC_H
#define BENCHMARKING_CREATEOBJECTSBYFOURCC_H
//------------------------------------------------------------------------------
/**
    @class Benchmarking::CreateObjectsByFourCC
  
    Create many objects by their class FourCC code.
    
    (C) 2006 Radon Labs GmbH
*/
#include "benchmarkbase/benchmark.h"

//------------------------------------------------------------------------------
namespace Benchmarking
{
class CreateObjectsByFourCC : public Benchmark
{
    __DeclareClass(CreateObjectsByFourCC);
public:
    /// run the benchmark
    virtual void Run(Timing::Timer& timer);
};

} // namespace Benchmark
//------------------------------------------------------------------------------
#endif
