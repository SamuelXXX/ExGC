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

    void wait_show_debug()
    {
        exgc::Profile(1);
        std::cout<<"Press Any Key to Continue......\n\n\n"<<std::endl;
        getchar(); 
    }

    void unit_test()
    {
        g_A=new A();
        g_A->anotherRef=new A();
        g_A->anotherRef->anotherRef=new A();
        g_A->anotherRef->anotherRef->anotherRef=g_A->anotherRef;

        wait_show_debug();
        exgc::Collect(1);
        wait_show_debug();
    }

    void Test()
    {
        unit_test();

        wait_show_debug();
        g_A=nullptr;

        wait_show_debug();

        exgc::Collect(1);
        wait_show_debug();
    }
}