#include "ExGCTestbench.h"
#include<string>
#include<vector>

namespace ExGC::Testbench::ReferenceCounter
{
    class Foo:public GCObject
    {
        char m_name[50];

        public:
        Foo(const std::string&);
        const char* Name()
        {
            return m_name;
        }
        void GCTrackReference(GCPoolVisitor& v) override;
    };

    Foo::Foo(const std::string& str)
    {
        strcpy(m_name,str.c_str());
    }

    BeginCaptureRef(Foo)
    EndCaptureRef

    std::vector<Ref<Foo>> list;

    void unit_test()
    {
        Ref<Foo> foo=new Foo("ReferenceCounterTest");

        if(list.size()>100)
        {
            AssertTotalGCSize(list.size()+1);
            list.clear();
        }
        list.push_back(foo);
    }

    void Test()
    {
        int count=100000;
        std::cout<<"ReferenceCounter Test:"<<count<<std::endl;
        for(int i=0;i<count;i++)
            unit_test();
        
        ExGC::Profile();
        list.clear();
        AssertTotalGCSize(0);
        std::cout<<"ReferenceCounter Finish\n"<<std::endl;
    }
}