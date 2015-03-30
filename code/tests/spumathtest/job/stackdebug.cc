//------------------------------------------------------------------------------
//  stackdebug.cc
//  (C) 2009 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stackdebug.h"
#include <spu_printf.h>
#include <cassert>
#include <cstring>

using namespace Test;

static const int CHECKPOINTS_MAX = 40;
static const int MESSAGE_LEN_MAX = 50;
struct Checkpoint
{
    char msg[MESSAGE_LEN_MAX];
    volatile unsigned int *stack_ptr;
};
static Checkpoint checkpoints[CHECKPOINTS_MAX];
static int checkpointCount = 0;

void Test::StackCheckpoint(const char *msg, volatile unsigned int *stack_ptr)
{
    assert(checkpointCount < CHECKPOINTS_MAX);
    Checkpoint *ck = &checkpoints[checkpointCount++];
    std::strncpy(ck->msg, msg, MESSAGE_LEN_MAX);
    ck->msg[MESSAGE_LEN_MAX - 1] = '\0';
    ck->stack_ptr = stack_ptr;
}

void Test::DumpStackCheckpoints()
{
    spu_printf("\n\nStack Checkpoints:\n\n");
    register volatile const unsigned int *stack_size asm("2");
    unsigned int tmp = *stack_size;
    spu_printf("%u\n", tmp);
    spu_printf("\tStack Size: 0x%x  %u byte\n\n", tmp, tmp);
    int i;
    for(i = 0; i < checkpointCount; ++i)
    {
        spu_printf("\t[%-35s] stack ptr: 0x%p\n", checkpoints[i].msg, checkpoints[i].stack_ptr);
    }
}
