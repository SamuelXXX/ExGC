#include<iostream>
#include"exgc_testbench.h"
#include<vector>

using namespace exgc;

namespace exgc::testbench::circular_reference
{
    class B;
    class A:public GCObject
    {
        char m_bulk[1024*1024];
        
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

    std::vector<Ref<A>> refAList;

    void unit_test()
    {
        Ref<A> a=new A();
        Ref<B> b=new B();
        refAList.push_back(a);
        a->m_ob=b;
        b->m_ob=a;
    }

    bool Test()
    {
        int count=200;
        for(int j=0;j<20;j++)
        {
            for(int i=0;i<count;++i)
		        unit_test();
            
            // exgc::Collect(1);
            // exgc::Profile(1);
            // AssertGCSize(1,2);
        }
        
        exgc::Profile();
        // AssertGCSize(1,2);
        refAList.clear();
        exgc::Collect();
        exgc::Profile();
        return GCSizeCondition(1,0);
    }
}