#include "ExGC.h"

namespace exgc
{
    static GCGenerationManager * _gc_manager=nullptr;

    GCGenerationManager *GCGenerationManager::GetInstance()
    {
        if(_gc_manager==nullptr)
        {
            _gc_manager=new GCGenerationManager();
        }

        return _gc_manager;
    }

    GCGenerationManager::GCGenerationManager():m_wild(0,0),m_gen1(1,1024),m_gen2(2,2048),m_gen3(3,0),m_refCounterFlag(true)
    {

    }

    void GCGenerationManager::makeWild(GCPoolHeader *header_ptr)
    {
        assert(header_ptr->obGenId==InvalidGenID);
        m_wild.addNode(header_ptr);
    }

    void GCGenerationManager::makeManaged(GCPoolHeader *header_ptr)
    {
        m_wild.delNode(header_ptr);
        m_gen1.addNode(header_ptr);
    }

    void GCGenerationManager::ascend(GCPoolHeader *header_ptr)
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

    void GCGenerationManager::kick(GCPoolHeader *header_ptr)
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

    void GCGenerationManager::GCIncRef(GCObject *ob_ptr)
    {
        if(m_refCounterFlag)
        {
            ob_ptr->IncRef();
            GCPoolHeader *headerPtr=ExTractHeaderPtr(ob_ptr);
            if(headerPtr->obGenId==0) // Capture wild GCObject pointer to managed pool
            {
                makeManaged(headerPtr);
            }
        }
    }

    void GCGenerationManager::GCDecRef(GCObject *ob_ptr)
    {
        if(m_refCounterFlag)
        {
            ob_ptr->DecRef();
            if(ob_ptr->GetRefCount()==0)
            {
                delete ob_ptr;
            }
        }
    }

    void GCGenerationManager::Collect(int gen_index)
    {
        m_gen1.CollectPool(); // for test now
    }

    void GCGenerationManager::MemoryProfile()
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

    void GCGenerationManager::GenerationProfile(int index)
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

    void GCGenerationManager::ToggleReferenceCounter(bool flag)
    {
        m_refCounterFlag=flag;
    }

    size_t GCGenerationManager::GetGenerationSize(uint8_t genId)
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

    size_t GCGenerationManager::GetGenerationMemory(uint8_t genId)
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