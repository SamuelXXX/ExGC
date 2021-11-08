#include<iostream>
#include"../src/ExGC.h"

using namespace exgc;

namespace exgc::testbench::circular_reference
{
    class B;
    class A:public GCObject
    {
        char m_bulk[1024*1024*100];
        
        public:
        Ref<B> m_ob;
        void GCTrackReference() override
        {
            m_ob.Resolve();
        }
        
    };

    class B:public GCObject
    {
        public:
        Ref<A> m_ob;
        void GCTrackReference() override
        {
            m_ob.Resolve();
        }
    };

    void unit_test()
    {
        Ref<A> a=new A();
        Ref<B> b=new B();
        a->m_ob=b;
        b->m_ob=a;
    }

    void Test()
    {
        for(int i=0;i<10;++i)
		    unit_test();
	
        exgc::Profile(1);
        getchar();
        exgc::Collect(1);
        exgc::Profile(1);
        getchar();
    }
}