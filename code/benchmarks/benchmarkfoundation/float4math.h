#pragma once
#ifndef BENCHMARKING_FLOAT4MATH_H
#define BENCHMARKING_FLOAT4MATH_H
//------------------------------------------------------------------------------
/**
    @class Benchmarking::Float4Math
    
    Test float4 misc functions performance.
    
    (C) 2007 Radon Labs GmbH
*/
#include "benchmarkbase/benchmark.h"

//------------------------------------------------------------------------------
namespace Benchmarking
{
class Float4Math : public Benchmark
{
    __DeclareClass(Float4Math);
public:
    /// run the benchmark
    virtual void Run(Timing::Timer& timer);
};

} // namespace Benchmark
//------------------------------------------------------------------------------
#endif
