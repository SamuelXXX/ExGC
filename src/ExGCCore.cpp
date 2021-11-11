#include "ExGC.h"
#include<assert.h>
#include<time.h>

namespace ExGC
{
    static GCCore * _gc_core=nullptr;

    GCCore *GCCore::GetInstance()
    {
        if(_gc_core==nullptr)
            _gc_core=new GCCore();
        
        return _gc_core;
    }

    GCCore::GCCore():
    m_wildPool(WildGenID,0),
    m_enableRefCounter(true),
    m_generations{GCPool(0,1024),GCPool(1,0),GCPool(2,0)}
    {
    }

    void GCCore::_collectPool(uint8_t genIndex)
    {
        GCPool& pool=m_generations[genIndex];
        // Reset transfer times
        pool.m_transferTimes=0; 

        if(pool.m_currentSize==0)
            return;

        size_t sizeBeforeCollect=pool.m_currentSize;
        clock_t timeBeforeCollect=clock();
        clock_t timeFree=0;

        // Disable auto inc-dec refcnt, or delete operation or 'GCTrackReference' may cause chain reaction of destructing GCObjects
        m_enableRefCounter=false; 

        // Initialize all external reference count value
        GCPoolHeader *cursorPtr=pool.head;
        while (cursorPtr)
        {
            GCObject *obPtr=ExTractObjectPtr(cursorPtr);
            cursorPtr->extRefcnt=cursorPtr->obRefcnt;
            cursorPtr=cursorPtr->next;
        }

        GCPoolVisitor visitor(GCPoolVisitor::VisitStrategy::CalExtRefCnt,pool.m_genId);

        // Generate all external reference count value
        cursorPtr=pool.head;
        while (cursorPtr)
        {
            GCObject *obPtr=ExTractObjectPtr(cursorPtr);
            obPtr->GCTrackReference(visitor);
            cursorPtr=cursorPtr->next;
        }

        // Initializing tracking state
        cursorPtr=pool.head;
        while (cursorPtr)
        {
            if(cursorPtr->extRefcnt>0)
            {
                cursorPtr->trackState.trackRoot=true;
                cursorPtr->trackState.reachable=true;
            }
            else
            {
                cursorPtr->trackState.trackRoot=false;
                cursorPtr->trackState.reachable=false;
            }
            cursorPtr=cursorPtr->next;
        }

        // Tracking reachable objects
        visitor=GCPoolVisitor(GCPoolVisitor::VisitStrategy::TraceReachable,pool.m_genId);
        cursorPtr=pool.head;
        while (cursorPtr)
        {
            GCObject *obPtr=ExTractObjectPtr(cursorPtr);
            if(cursorPtr->trackState.trackRoot)
            {
                obPtr->GCTrackReference(visitor);
            }
            cursorPtr=cursorPtr->next;
        }

        // Deleting unreachable objects
        cursorPtr=pool.head;
        while (cursorPtr)
        {
            GCObject *obPtr=ExTractObjectPtr(cursorPtr);
            GCPoolHeader *nextPtr=cursorPtr->next;

            if(!cursorPtr->trackState.reachable)
            {
                pool.delNode(cursorPtr);
                cursorPtr->obRefcnt=0; // Reset reference before delete target GCObject
                clock_t temClock=clock();
                delete obPtr;
                timeFree+=clock()-temClock;
            }

            cursorPtr=nextPtr;
        }

        m_enableRefCounter=true; // Resume auto inc-dec refcnt

        size_t sizeCollected=sizeBeforeCollect-pool.m_currentSize;
        clock_t timeCollected=clock()-timeBeforeCollect;
        double time=(timeCollected-timeFree)*1.0/CLOCKS_PER_SEC*1000;
        GCLog(nullptr,std::to_string(sizeCollected)+" Objects Collected in Generation "+std::to_string(pool.m_genId)+" within "+std::to_string(time)+" milliseconds!");  
    }

    void GCCore::_transferPool(uint8_t fromIndex,uint8_t toIndex)
    {
        m_generations[fromIndex].transfer(m_generations[toIndex]);
        ++m_generations[toIndex].m_transferTimes; // Increase transfer times
    }

    bool GCCore::_shouldCollect(uint8_t genIndex)
    {
        if(m_generations[genIndex].m_transferTimes>=10) // over transfer times
            return true;
        
        if(m_generations[genIndex].m_collectThreshold!=0&&
        m_generations[genIndex].m_currentSize>m_generations[genIndex].m_collectThreshold) // oversize
        {
            return true;
        }
        return false;
    }

    void GCCore::GCIncRef(GCObject *ob_ptr)
    {
        if(m_enableRefCounter)
        {
            GCPoolHeader *headerPtr=ExTractHeaderPtr(ob_ptr);
            ++headerPtr->obRefcnt;
            if(headerPtr->obGenId==WildGenID) // Capture wild GCObject pointer to managed pool
            {
                m_wildPool.delNode(headerPtr);
                m_generations[0].addNode(headerPtr);
                
                if(_shouldCollect(0))
                {
                    _collectPool(0);
                    _transferPool(0,1);

                    if(_shouldCollect(1))
                    {
                        _collectPool(1);
                        _transferPool(1,2);

                        if(_shouldCollect(2))
                        {
                            _collectPool(2);
                        }
                    }
                }
            }
        }
    }

    void GCCore::GCDecRef(GCObject *ob_ptr)
    {
        if(m_enableRefCounter)
        {
            GCPoolHeader *headerPtr=ExTractHeaderPtr(ob_ptr);
            --headerPtr->obRefcnt;
            if(headerPtr->obRefcnt==0)
            {
                delete ob_ptr;
            }
        }
    }

    void GCCore::Collect(uint8_t gen_index)
    {
        // m_gen1.CollectPool(); // for test now
        if(gen_index>=3)
        {
            GCLog(nullptr,"Invalid generation index when calling Collect");
            return;
        }

        if(gen_index==0)
        {
            _collectPool(0);
            _transferPool(0,1);

            if(_shouldCollect(1))
            {
                _collectPool(1);
                _transferPool(1,2);

                if(_shouldCollect(2))
                {
                    _collectPool(2);
                }
            }
        }

        if(gen_index==1)
        {
            _collectPool(0);
            _transferPool(0,1);
            _collectPool(1);
            _transferPool(1,2);
            if(_shouldCollect(2))
            {
                _collectPool(2);
            }
        }

        if(gen_index==2)
        {
            _collectPool(0);
            _transferPool(0,1);
            _collectPool(1);
            _transferPool(1,2);
            _collectPool(2);
        }
    }

    void *GCCore::Malloc(size_t size)
    {
        // Assign space and make connection to pool
        void *ptr=std::malloc(size+sizeof(GCPoolHeader));
        GCPoolHeader *header_ptr=(GCPoolHeader *)ptr;
        GCObject *ob_ptr=(GCObject *)((uint8_t *)ptr+sizeof(GCPoolHeader));

        header_ptr->obRefcnt=0;
        header_ptr->obSize=size;
        header_ptr->obGenId=InvalidGenID;

        m_wildPool.addNode(header_ptr);

        return ob_ptr;
    }

    void GCCore::Free(void * ptr)
    {
        GCObject *ob_ptr=(GCObject *)ptr;
        GCPoolHeader *header_ptr=(GCPoolHeader *)((uint8_t *)ptr-sizeof(GCPoolHeader));
        assert(header_ptr->obRefcnt==0);
        
        if(header_ptr->obGenId!=InvalidGenID) // Still in pool
        {
            if(header_ptr->obGenId==WildGenID)
            {
                m_wildPool.delNode(header_ptr);
            }
            else
            {
                m_generations[header_ptr->obGenId].delNode(header_ptr);
            }
            
        }
            
        std::free(header_ptr);
    }

    void GCCore::MemoryProfile()
    {
        size_t totalMem=m_wildPool.GetGCObjectMemory();
        for(int gi=0;gi<3;gi++)
        {
            totalMem+=m_generations[gi].GetGCObjectMemory();
        }
        GCLog(nullptr, "TotalMem:"+std::to_string(totalMem));
        GCLog(nullptr, "Wild_Mem:"+std::to_string(m_wildPool.GetGCObjectMemory()));
        for(int gi=0;gi<3;gi++)
        {
            GCLog(nullptr, "Gen"+std::to_string(gi)+"_Mem:"+std::to_string(m_generations[gi].GetGCObjectMemory()));
        }
    }

    void GCCore::GenerationProfile(int index)
    {
        GCLog(nullptr, "GenProfile-"+std::to_string(index));
        m_generations[index].Profile();
        GCLog(nullptr, "EndGenProfile-"+std::to_string(index));
    }

    size_t GCCore::GetGenerationSize(uint8_t genId)
    {
        return m_generations[genId].GetSize();
    }

    size_t GCCore::GetGenerationMemory(uint8_t genId)
    {
        return m_generations[genId].GetGCObjectMemory();
    }

    size_t GCCore::GetTotalSize()
    {
        size_t size=0;
        for(int gi=0;gi<3;++gi)
        {
            size+=m_generations[gi].GetSize();
        }
        return size;
    }
    size_t GCCore::GetTotalMemory()
    {
        size_t size=0;
        for(int gi=0;gi<3;++gi)
        {
            size+=m_generations[gi].GetGCObjectMemory()+m_generations[gi].GetSize()*sizeof(GCPoolHeader);
        }
        return size;
    }
}