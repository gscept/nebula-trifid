#pragma once
#ifndef BENCHMARKING_MATRIX44MULTIPLY_H
#define BENCHMARKING_MATRIX44MULTIPLY_H
//------------------------------------------------------------------------------
/**
    @class Benchmarking::Matrix44Multiply
    
    Test matrix44::multiply() performance.
    
    (C) 2007 Radon Labs GmbH
*/
#include "benchmarkbase/benchmark.h"

//------------------------------------------------------------------------------
namespace Benchmarking
{
class Matrix44Multiply : public Benchmark
{
    __DeclareClass(Matrix44Multiply);
public:
    /// run the benchmark
    virtual void Run(Timing::Timer& timer);
};

} // namespace Benchmark
//------------------------------------------------------------------------------
#endif
