#include "ExGC.h"
#include<assert.h>

namespace exgc
{
    static GCCore * _gc_core=nullptr;

    GCCore *GCCore::GetInstance()
    {
        if(_gc_core==nullptr)
            _gc_core=new GCCore();
        
        return _gc_core;
    }

    GCCore::GCCore():m_wild(0,0),m_gen1(1,1024),m_gen2(2,2048),m_gen3(3,4096),m_refCounterFlag(true){}

    void GCCore::makeWild(GCPoolHeader *header_ptr)
    {
        assert(header_ptr->obGenId==InvalidGenID);
        m_wild.addNode(header_ptr);
    }

    void GCCore::makeManaged(GCPoolHeader *header_ptr)
    {
        m_wild.delNode(header_ptr);
        m_gen1.addNode(header_ptr);
        if(m_gen1.ShouldGC())
        {
            m_gen1.CollectPool();
            m_gen2.linkNodes(m_gen1.clearNodes());
            if(m_gen2.ShouldGC())
            {
                m_gen2.CollectPool();
                m_gen3.linkNodes(m_gen2.clearNodes());
                if(m_gen3.ShouldGC()) // The 3rd generation is lazy triggered
                {
                    m_gen3.CollectPool(); 
                }
            } 
        }
    }

    void GCCore::ascend(GCPoolHeader *header_ptr)
    {
        uint8_t curGenId=header_ptr->obGenId;
        switch(curGenId)
        {
            case 0:m_wild.delNode(header_ptr);break;
            case 1:m_gen1.delNode(header_ptr);break;
            case 2:m_gen2.delNode(header_ptr);break;
            case 3:break;
            default:throw "GenID unknown:"+std::to_string(curGenId);
        }

        switch(curGenId+1)
        {
            case 1:m_gen1.addNode(header_ptr);break;
            case 2:m_gen2.addNode(header_ptr);break;
            case 3:m_gen3.addNode(header_ptr);break;
            default:break;
        }
    }

    void GCCore::kick(GCPoolHeader *header_ptr)
    {
        uint8_t curGenId=header_ptr->obGenId;
        switch(curGenId)
        {
            case 0:m_wild.delNode(header_ptr);break;
            case 1:m_gen1.delNode(header_ptr);break;
            case 2:m_gen2.delNode(header_ptr);break;
            case 3:m_gen3.delNode(header_ptr);break;
            default:break; // Already wild
        }
    }

    void GCCore::GCIncRef(GCObject *ob_ptr)
    {
        if(m_refCounterFlag)
        {
            ++ob_ptr->m_refcnt;
            GCPoolHeader *headerPtr=ExTractHeaderPtr(ob_ptr);
            if(headerPtr->obGenId==0) // Capture wild GCObject pointer to managed pool
            {
                makeManaged(headerPtr);
            }
        }
    }

    void GCCore::GCDecRef(GCObject *ob_ptr)
    {
        if(m_refCounterFlag)
        {
            --ob_ptr->m_refcnt;
            if(ob_ptr->GetRefCount()==0)
            {
                delete ob_ptr;
            }
        }
    }

    void GCCore::Collect(int gen_index)
    {
        // m_gen1.CollectPool(); // for test now
        if(gen_index<1||gen_index>3)
        {
            GCLog(nullptr,"Invalid generation index when calling Collect");
            return;
        }

        if(gen_index==1) // Only Collect Generation 1
        {
            m_gen1.CollectPool();
            m_gen2.linkNodes(m_gen1.clearNodes());
            if(m_gen2.ShouldGC())
            {
                m_gen2.CollectPool();
                m_gen3.linkNodes(m_gen2.clearNodes());
                if(m_gen3.ShouldGC()) 
                {
                    m_gen3.CollectPool(); 
                }
            }
        }
        else if(gen_index==2) // Collect Generation 1 and Generation 2
        {
            m_gen1.CollectPool();
            m_gen2.linkNodes(m_gen1.clearNodes());
            m_gen2.CollectPool();
            m_gen3.linkNodes(m_gen2.clearNodes());
            if(m_gen3.ShouldGC()) 
            {
                m_gen3.CollectPool(); 
            }     
        }
        else if(gen_index==3) // Collect all 3 generations
        {
            m_gen1.CollectPool();
            m_gen2.linkNodes(m_gen1.clearNodes());
            m_gen2.CollectPool();
            m_gen3.linkNodes(m_gen2.clearNodes());
            m_gen3.CollectPool();   
        }
    }

    void GCCore::MemoryProfile()
    {
        size_t wild_mem=m_wild.GetGCObjectMemory();
        size_t gen1_mem=m_gen1.GetGCObjectMemory();
        size_t gen2_mem=m_gen2.GetGCObjectMemory();
        size_t gen3_mem=m_gen3.GetGCObjectMemory();
        GCLog(nullptr, "TotalMem:"+std::to_string(wild_mem+gen1_mem+gen2_mem+gen3_mem));
        GCLog(nullptr, "Wild_Mem:"+std::to_string(wild_mem));
        GCLog(nullptr, "Gen1_Mem:"+std::to_string(gen1_mem));
        GCLog(nullptr, "Gen2_Mem:"+std::to_string(gen2_mem));
        GCLog(nullptr, "Gen3_Mem:"+std::to_string(gen3_mem));
    }

    void GCCore::GenerationProfile(int index)
    {
        GCLog(nullptr, "GenProfile-"+std::to_string(index));
        switch(index)
        {
            case 0:m_wild.Profile();break;
            case 1:m_gen1.Profile();break;
            case 2:m_gen2.Profile();break;
            case 3:m_gen3.Profile();break;
            default:break;
        }
        GCLog(nullptr, "EndGenProfile-"+std::to_string(index));
    }

    void GCCore::ToggleReferenceCounter(bool flag)
    {
        m_refCounterFlag=flag;
    }

    size_t GCCore::GetGenerationSize(uint8_t genId)
    {
        switch(genId)
        {
            case 0:return m_wild.GetSize();
            case 1:return m_gen1.GetSize();
            case 2:return m_gen2.GetSize();
            case 3:return m_gen3.GetSize();
            default:return 0;
        }
        return 0;
    }

    size_t GCCore::GetGenerationMemory(uint8_t genId)
    {
        switch(genId)
        {
            case 0:return m_wild.GetGCObjectMemory();
            case 1:return m_gen1.GetGCObjectMemory();
            case 2:return m_gen2.GetGCObjectMemory();
            case 3:return m_gen3.GetGCObjectMemory();
            default:return 0;
        }
        return 0;
    }
}