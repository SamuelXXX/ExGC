#include<iostream>
#include"exgc_testbench.h"

using namespace exgc;

namespace exgc::testbench::circular_reference
{
    class B;
    class A:public GCObject
    {
        char m_bulk[10];
        
        public:
        Ref<B> m_ob;
        void GCTrackReference(GCPoolVisitor& v) override
        {
            v.Visit(m_ob);
        }
        ~A()
        {
            
        }
        
    };

    class B:public GCObject
    {
        public:
        Ref<A> m_ob;
        void GCTrackReference(GCPoolVisitor& v) override
        {
            v.Visit(m_ob);
        }

        ~B()
        {

        }
    };

    Ref<A> gA;

    void unit_test()
    {
        Ref<A> a=new A();
        Ref<B> b=new B();
        gA=a;
        a->m_ob=b;
        b->m_ob=a;
    }

    bool Test()
    {
        int count=2000000;
        for(int j=0;j<20;j++)
        {
            for(int i=0;i<count;++i)
		        unit_test();
            
            exgc::Collect(1);
            // exgc::Profile(1);
            AssertGCSize(1,2);
        }
        
        // exgc::Profile(1);
        // AssertGCSize(1,2);
        gA=nullptr;
        exgc::Collect(1);
        exgc::Profile(1);
        return GCSizeCondition(1,0);
    }
}