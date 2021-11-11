#include<iostream>
#include"ExGCTestbench.h"
#include<vector>

using namespace ExGC;

namespace ExGC::Testbench::AutoCollect
{
    template <class T>
    class List:public std::vector<Ref<T>>,public IGCReferenceType
    {
        public:
        void OnRefVisited(GCPoolVisitor &v) override
        {
            for(auto ref:*this)
            {
                v.Visit(ref);
            }
        }
    };

    class SelfRef:public GCObject
    {
        char m_blob[20];
        
        public:
        Ref<SelfRef> self;
        void GCTrackReference(GCPoolVisitor& v) override
        {
            v.Visit(self);
        }

    };

    List<SelfRef> list;

    void unit_test()
    {
        // Careful!!!This will trigger a very rare bug!!!!!!!!!!!!!!!!!
        // New object constructing process direct or indirect ref itself may trigger auto gc process to destoy this object
        // We'd better move auto gc process out of GCIncRef method
        Ref<SelfRef> a=new SelfRef(); // May destroy self when created
        a->self=a;
        list.push_back(a);
    }

    void Test()
    {
        int count=100;
        ExGC::SetCollectThreshold(8);
        std::cout<<"AutoCollect Test:"<<count<<std::endl;
        for(int i=0;i<count;i++)
            unit_test();
        
        ExGC::Profile();
        list.clear();
        ExGC::Collect();
        AssertTotalGCSize(0);
        std::cout<<"AutoCollect Finish\n"<<std::endl;

        ExGC::SetCollectThreshold(1024);
    }
}