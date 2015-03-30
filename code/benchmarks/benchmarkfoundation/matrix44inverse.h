#pragma once
#ifndef BENCHMARKING_MATRIX44INVERSE_H
#define BENCHMARKING_MATRIX44INVERSE_H
//------------------------------------------------------------------------------
/**
    @class Benchmarking::Matrix44Inverse
    
    Test matrix44::inverse() performance.
    
    (C) 2007 Radon Labs GmbH
*/
#include "benchmarkbase/benchmark.h"

//------------------------------------------------------------------------------
namespace Benchmarking
{
class Matrix44Inverse : public Benchmark
{
    __DeclareClass(Matrix44Inverse);
public:
    /// run the benchmark
    virtual void Run(Timing::Timer& timer);
};

} // namespace Benchmark
//------------------------------------------------------------------------------
#endif
