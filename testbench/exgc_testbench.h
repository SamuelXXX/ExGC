#ifndef __EXGC_TESTBENCH_H__
#define __EXGC_TESTBENCH_H__

#include<assert.h>
#include"../src/ExGC.h"

#define GCSizeCondition(genId,size) (ExGC::GCCore::GetInstance()->GetGenerationSize((genId))==(size))
#define AssertGCSize(genId,size) assert(ExGC::GCCore::GetInstance()->GetGenerationSize((genId))==(size))
namespace ExGC::Testbench
{
    namespace Simple
    {
        bool Test();
    }

    namespace CircularReference
    {
        bool Test();
    }

    namespace GameRuntime
    {
        bool Test();
    }
}

#endif