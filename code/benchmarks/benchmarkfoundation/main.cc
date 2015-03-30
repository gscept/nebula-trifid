//------------------------------------------------------------------------------
//  runbenchmarks.cc
//  (C) 2006 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "core/coreserver.h"
#include "core/sysfunc.h"
#include "benchmarkbase/benchmarkrunner.h"

#include "createobjects.h"
#include "createobjectsbyfourcc.h"
#include "createobjectsbyclassname.h"
#include "float4math.h"
#include "matrix44inverse.h"
#include "matrix44multiply.h"
#include "mempoolbenchmark.h"

using namespace Core;
using namespace Benchmarking;

void __cdecl
main()
{
    // create Nebula3 runtime
    Ptr<CoreServer> coreServer = CoreServer::Create();
    coreServer->SetAppName(Util::StringAtom("Nebula3 Benchmark Runner"));
    coreServer->Open();

    // setup and run benchmarks
    Ptr<BenchmarkRunner> runner = BenchmarkRunner::Create();    
//    runner->AttachBenchmark(Matrix44Multiply::Create());
//    runner->AttachBenchmark(Matrix44Inverse::Create());
//    runner->AttachBenchmark(Float4Math::Create());
    runner->AttachBenchmark(MemPoolBenchmark::Create());
    runner->AttachBenchmark(CreateObjects::Create());
    runner->AttachBenchmark(CreateObjectsByFourCC::Create());
    runner->AttachBenchmark(CreateObjectsByClassName::Create());
    runner->Run();
    
    // shutdown Nebula3 runtime
    runner = 0;
    coreServer->Close();
    coreServer = 0;
    SysFunc::Exit(0);
}
