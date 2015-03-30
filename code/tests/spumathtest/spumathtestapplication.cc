//------------------------------------------------------------------------------
//  jobstestapplication.cc
//  (C) 2009 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "spumathtestapplication.h"
#include "math/matrix44.h"
#include "math/float4.h"

namespace Test
{
using namespace Jobs;
using namespace Math;

extern "C" {
    extern const char _binary_jqjob_spumathtest_job_ps3_spu_bin_start[];
    extern const char _binary_jqjob_spumathtest_job_ps3_spu_bin_size[];
}

//------------------------------------------------------------------------------
/**
*/
JobsTestApplication::JobsTestApplication()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
JobsTestApplication::~JobsTestApplication()
{
    n_assert(!this->IsOpen());
}

//------------------------------------------------------------------------------
/**
*/
bool
JobsTestApplication::Open()
{
    if (ConsoleApplication::Open())
    {
        // setup jobs subsystem
        this->jobSystem = JobSystem::Create();
        this->jobSystem->Setup();

        this->gameContentServer = IO::GameContentServer::Create();
        this->gameContentServer->SetTitle("RL Test Title");
        this->gameContentServer->SetTitleId("RLTITLEID");
        this->gameContentServer->SetVersion("1.00");
        this->gameContentServer->Setup();

        return true;
    }
    else
    {
        return false;
    }
}

//------------------------------------------------------------------------------
/**
*/
void
JobsTestApplication::Close()
{
    this->gameContentServer->Discard();
    this->gameContentServer = 0;
    this->jobSystem->Discard();
    this->jobSystem = 0;
    ConsoleApplication::Close();
}

//------------------------------------------------------------------------------
/**
*/
void
JobsTestApplication::Run()
{
    JobFuncDesc jobFuncDesc(_binary_jqjob_spumathtest_job_ps3_spu_bin_start, 
                            _binary_jqjob_spumathtest_job_ps3_spu_bin_size);

    // setup a job port
    this->jobPort = JobPort::Create();
    this->jobPort->Setup();

    const int SLICE_ELEMENTS = 16;
    // NOTE: uniform data must not be on the stack!!!
    //SizeT numElements = 100000;
    SizeT numElements = 16;
    float4* srcBuffer = (float4*) Memory::Alloc(Memory::ResourceHeap, numElements * sizeof(float4));
    float4* dstBuffer = (float4*) Memory::Alloc(Memory::ResourceHeap, numElements * sizeof(float4));
    //float4* finalBuffer = (float4*) Memory::Alloc(Memory::ResourceHeap, numElements * sizeof(float4));
    matrix44* uniBuffer = (matrix44*) Memory::Alloc(Memory::ScratchHeap, sizeof(matrix44));
    float4 v(0.0f, 0.0f, 0.0f, 1.0f);
    IndexT i;
    for (i = 0; i < numElements; i++)
    {
        srcBuffer[i] = v;
    }
    matrix44 m = matrix44::translation(1.0f, 2.0f, 3.0f);
    uniBuffer[0] = m;

    Ptr<Job> job = Job::Create();
    job->Setup(JobUniformDesc(uniBuffer, sizeof(matrix44), 0),
               JobDataDesc(srcBuffer, sizeof(float4) * numElements, sizeof(float4) * SLICE_ELEMENTS),
               JobDataDesc(dstBuffer, sizeof(float4) * numElements, sizeof(float4) * SLICE_ELEMENTS),
               jobFuncDesc);


    this->jobPort->PushJob(job);
    this->jobPort->WaitDone();

    job->Discard();
    job = 0;
    this->jobPort->Discard();
    this->jobPort = 0;
}

} // namespace Test