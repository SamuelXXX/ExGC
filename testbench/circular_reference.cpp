#include<iostream>
#include"exgc_testbench.h"
#include<vector>

using namespace ExGC;

namespace ExGC::Testbench::CircularReference
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

    void Test()
    {
        int count=20;
        for(int j=0;j<20;j++)
        {
            for(int i=0;i<count;++i)
		        unit_test();
        }
        
        ExGC::Profile();
        AssertGCSize(0,800);
        refAList.clear();
        ExGC::Collect();
        ExGC::Profile();
        AssertGCSize(0,0);
        AssertGCSize(1,0);
        AssertGCSize(2,0);
    }
}