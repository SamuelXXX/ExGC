#include<iostream>
#include"../src/ExGC.h"

using namespace exgc;

namespace exgc::testbench::simple
{
    class A:public GCObject
    {
        char m_blob[100*1024*1024];
        public:
        void GCTrackReference() override
        {

        }
    };

    Ref<A> g_A;

    void wait_show_debug()
    {
        exgc::DebugLog(1);
        std::cout<<"Press Any Key to Continue......\n\n\n"<<std::endl;
        getchar(); 
    }

    void unit_test()
    {
        g_A=new A();
        Ref<A> l_A=new A();
        Ref<A> rg_A=g_A;
        exgc::Collect(1);
        wait_show_debug();
    }

    void Test()
    {
        unit_test();

        wait_show_debug();

        g_A=nullptr;

        wait_show_debug();
    }
}