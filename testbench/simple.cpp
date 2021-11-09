#include<iostream>
#include"exgc_testbench.h"

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
        AssertGCSize(1,3);
        g_A=nullptr;
        exgc::Collect(1);
        exgc::Profile(1);
        return GCSizeCondition(1,0);
    }
}