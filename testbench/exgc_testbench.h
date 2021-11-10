#ifndef __EXGC_TESTBENCH_H__
#define __EXGC_TESTBENCH_H__

#include<assert.h>
#include"../src/ExGC.h"

#define GCSizeCondition(genId,size) (exgc::GCCore::GetInstance()->GetGenerationSize((genId))==(size))
#define AssertGCSize(genId,size) assert(exgc::GCCore::GetInstance()->GetGenerationSize((genId))==(size))
namespace exgc::testbench
{
    namespace simple
    {
        bool Test();
    }

    namespace circular_reference
    {
        bool Test();
    }

    namespace game_runtime
    {
        bool Test();
    }
}

#endif