#ifndef __EXGC_TESTBENCH_H__
#define __EXGC_TESTBENCH_H__

#include<iostream>
#include"../src/ExGC.h"

#define ExGCAssert(expression,message)  if(!(expression))\
                                        {\
                                            std::cout<<"ExGCAssertion Failed!!! @"<<__FILE__<<":"<<__LINE__<<std::endl;\
                                            std::cout<<message<<std::endl;\
                                            std::cout<<"Press any key to exit!"<<std::endl;\
                                            getchar();\
                                            exit(0);\
                                        }
#define GCSizeCondition(genId,size) (ExGC::GCCore::GetInstance()->GetGenerationSize((genId))==(size))
#define AssertGCSize(genId,size) ExGCAssert(ExGC::GCCore::GetInstance()->GetGenerationSize((genId))==(size),"GC Generation Pool Size Assertion Failed!")
#define AssertTotalGCSize(size) ExGCAssert(ExGC::GCCore::GetInstance()->GetTotalSize()==(size),"GC Total Size Assertion Failed!")
#define AssertTotalGCMem(size) ExGCAssert(ExGC::GCCore::GetInstance()->GetTotalMemory()==(size),"GC Total Memory Assertion Failed!")
namespace ExGC::Testbench
{
    namespace ReferenceCounter
    {
        void Test();
    }

    namespace AutoCollect
    {
        void Test();
    }

    namespace CircularReference
    {
        void Test();
    }

    namespace GameRuntime
    {
        void Test();
    }
}

#endif