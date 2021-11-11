#include<iostream>
#include"ExGCTestbench.h"
#include<vector>
#include<random>

using namespace ExGC;

namespace ExGC::Testbench::CircularReference
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

    class GraphNode:public GCObject
    {
        char m_name[128];

        public:
        List<GraphNode> forwardNodes;
        GraphNode(const std::string&);
        const char* Name()
        {
            return m_name;
        }
        void GCTrackReference(GCPoolVisitor& v) override;
    };

    GraphNode::GraphNode(const std::string& str)
    {
        strcpy(m_name,str.c_str());
    }

    BeginCaptureRef(GraphNode)
        CaptureRef(forwardNodes)
    EndCaptureRef

    
    // std::default_random_engine random;

    void unit_test()
    {
        int nodeCount=50;
        List<GraphNode> nodesList;
        for(int i=0;i<nodeCount;i++)
            nodesList.push_back(new GraphNode("CircularReference"));
        
        for(auto ref:nodesList)
        {
            for(int i=0;i<2;++i)
            {
                ref->forwardNodes.push_back(nodesList[std::rand()%nodeCount]);
            }
        }
    }

    void Test()
    {
        int count=10;
        std::cout<<"CircularReference Test:"<<count<<std::endl;
        for(int i=0;i<count;i++)
            unit_test();
        
        ExGC::Profile();
        ExGC::Collect();
        AssertTotalGCSize(0);
        std::cout<<"CircularReference Finish\n"<<std::endl;
    }
}