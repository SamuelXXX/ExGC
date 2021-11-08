#include<iostream>
#include"../src/ExGC.h"

using namespace exgc;

namespace exgc::testbench::simple
{
    class A:public GCObject
    {
        Ref<A> m_self;
        char m_blob[1000*1024*1024];
        public:
        // A(const char *);

        void PrintName()
        {
            // std::cout<<m_blob<<std::endl;
        }

        void GCTrackReference() override
        {
            m_self.Resolve();
        }
    };

    // A::A(const char *name)
    // {
    //     strcpy(m_blob,name);
    //     m_self=this;
    // }

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
        Ref<A> l_A=new A();
        Ref<A> rg_A=g_A; 
        //exgc::Collect(1);
        wait_show_debug();
    }

    void Test()
    {
        unit_test();

        wait_show_debug();
        g_A->PrintName();
        g_A=nullptr;

        wait_show_debug();

        exgc::Collect(1);
        wait_show_debug();
    }
}