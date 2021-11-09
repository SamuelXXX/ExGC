#include<iostream>
#include"../src/ExGC.h"

using namespace exgc;

namespace exgc::testbench::simple
{
    class A:public GCObject
    {
        char m_blob[1000*1024*1024];
        public:
        Ref<A> anotherRef;
        void GCTrackReference(GCPoolVisitor& v) override
        {
            v.Visit(anotherRef);
        }

    };

    Ref<A> g_A;

    void unit_test()
    {
        g_A=new A();
        g_A->anotherRef=new A();
        g_A->anotherRef->anotherRef=new A();
        g_A->anotherRef->anotherRef->anotherRef=g_A->anotherRef;

        exgc::Collect(1);
    }

    bool Test()
    {
        unit_test();
        g_A=nullptr;
        exgc::Collect(1);
        exgc::Profile(1);
        return exgc::GCGenerationManager::GetInstance()->GetGenerationMemory(1)==0;
    }
}