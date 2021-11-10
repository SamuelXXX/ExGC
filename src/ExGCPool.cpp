#include "ExGC.h"
#include<iostream>
#include<string>
#include<time.h>
#include<assert.h>

namespace exgc
{
    GCPool::GCPool(uint8_t genId,size_t maxSize):
    m_genId(genId),
    m_collectThreshold(maxSize),
    head(nullptr),
    tail(nullptr),
    m_currentSize(0),
    m_currentMemory(0)
    {

    }

    GCPoolHeader *GCPool::addNode(GCPoolHeader *newNode)
    {
        newNode->obGenId=m_genId;
        m_currentSize+=1;
        m_currentMemory+=newNode->obSize;

        newNode->next=nullptr;
        newNode->prev=tail;

        if(head == nullptr)
        {
            head=newNode;
        }
        else
        {
            tail->next=newNode;
        }

        tail=newNode;

        return newNode;
    }

    GCPoolHeader *GCPool::delNode(GCPoolHeader *nodeDelete)
    {
        assert(nodeDelete->obGenId==m_genId&&m_currentSize!=0);

        nodeDelete->obGenId=InvalidGenID;
        m_currentSize-=1;
        m_currentMemory-=nodeDelete->obSize;

        GCPoolHeader *nextNode=nodeDelete->next;
        GCPoolHeader *prevNode=nodeDelete->prev;

        if(prevNode)
            prevNode->next=nextNode;
        
        if(nextNode)
            nextNode->prev=prevNode;
        
        if(head==nodeDelete)
            head=nextNode;
        
        if(tail==nodeDelete)
            tail=prevNode;
        
        return nextNode;
    }

    GCPoolHeader *GCPool::clearNodes()
    {
        GCPoolHeader *retHeader=head;

        // Reset this link list pool
        head=tail=nullptr;
        m_currentMemory=0;
        m_currentSize=0;

        // Reset genId of all nodes to InvalidGenID
        GCPoolHeader *cursor=retHeader;
        while (cursor)
        {
            cursor->obGenId=InvalidGenID;
            cursor=cursor->next;
        }
        
        return retHeader;
    }

    GCPoolHeader *GCPool::linkNodes(GCPoolHeader *anotherHeader)
    {
        if(anotherHeader==nullptr)
            return nullptr;
        
        if(head==nullptr)
        {
            head=anotherHeader;
        }
        else
        {
            tail->next=anotherHeader;
            anotherHeader->prev=tail;
        }

        tail=anotherHeader;
        while (true)
        {
            tail->obGenId=m_genId;
            m_currentSize+=1;
            m_currentMemory+=tail->obSize;
            if(tail->next==nullptr)
                break;
            tail=tail->next;
        }
        
        return tail;
    }

    size_t GCPool::GetGCObjectMemory()
    {
        return m_currentMemory;
    }

    size_t GCPool::CalcGCObjectMemory()
    {
        size_t memSize=0;
        GCPoolHeader *cursorPtr=head;
        while (cursorPtr)
        {
            memSize+=cursorPtr->obSize;
            cursorPtr=cursorPtr->next;
        }
        return memSize; 
    }

    size_t GCPool::GetSize()
    {
        return m_currentSize;
    }

    size_t GCPool::CalcSize()
    {
        size_t size=0;
        GCPoolHeader *cursorPtr=head;
        while (cursorPtr)
        {
            ++size;
            cursorPtr=cursorPtr->next;
        }
        return size; 
    }

    bool GCPool::Contain(GCPoolHeader *node)
    {
        return node->obGenId==m_genId;
    }

    bool GCPool::ShouldGC()
    {
        return m_currentSize>=m_collectThreshold;
    }

    void GCPool::CollectPool()
    {
        if(m_currentSize==0)
            return;

        size_t sizeBeforeCollect=m_currentSize;
        clock_t timeBeforeCollect=clock();
        clock_t timeFree=0;

        // Disable auto inc-dec refcnt, or delete operation or 'GCTrackReference' may cause chain reaction of destructing GCObjects
        GCCore::GetInstance()->ToggleReferenceCounter(false); 

        // Initialize all external reference count value
        GCPoolHeader *cursorPtr=head;
        while (cursorPtr)
        {
            GCObject *obPtr=ExTractObjectPtr(cursorPtr);
            cursorPtr->extRefcnt=cursorPtr->obRefcnt;
            cursorPtr=cursorPtr->next;
        }

        GCPoolVisitor visitor(GCPoolVisitor::VisitStrategy::TraceDirectRef,m_genId);

        // Generate all external reference count value
        cursorPtr=head;
        while (cursorPtr)
        {
            GCObject *obPtr=ExTractObjectPtr(cursorPtr);
            obPtr->GCTrackReference(visitor);
            cursorPtr=cursorPtr->next;
        }

        // Initializing tracking state
        cursorPtr=head;
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
        visitor=GCPoolVisitor(GCPoolVisitor::VisitStrategy::TraceIndirectRef,m_genId);
        cursorPtr=head;
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
        cursorPtr=head;
        while (cursorPtr)
        {
            GCObject *obPtr=ExTractObjectPtr(cursorPtr);
            GCPoolHeader *nextPtr=cursorPtr->next;

            if(!cursorPtr->trackState.reachable)
            {
                delNode(cursorPtr);
                cursorPtr->obRefcnt=0; // Reset reference before delete target GCObject
                clock_t temClock=clock();
                delete obPtr;
                timeFree+=clock()-temClock;
            }

            cursorPtr=nextPtr;
        }
        GCCore::GetInstance()->ToggleReferenceCounter(true); // Resume auto inc-dec refcnt

        size_t sizeCollected=sizeBeforeCollect-m_currentSize;
        clock_t timeCollected=clock()-timeBeforeCollect;
        double time=(timeCollected-timeFree)*1.0/CLOCKS_PER_SEC*1000;
        GCLog(nullptr,std::to_string(sizeCollected)+" Objects Collected in Generation "+std::to_string(m_genId)+" within "+std::to_string(time)+" milliseconds!");        
    }

    void GCPool::Profile()
    {
        assert(CalcGCObjectMemory()==m_currentMemory);
        assert(CalcSize()==m_currentSize);
        
        std::cout<<"\tMaxSize:"<<m_collectThreshold<<std::endl;
        std::cout<<"\tSize:"<<m_currentSize<<std::endl;
        std::cout<<"\tMemory:"<<m_currentMemory<<std::endl;
        if(m_currentSize>0)
        {
            std::cout<<"\t>>>>>>>>>>>>>>>>>>>>"<<std::endl;
            GCPoolHeader *cursor=head;
            int index=0;
            while(cursor)
            {
                GCObject *ob_ptr=ExTractObjectPtr(cursor);
                std::cout<<"\t\t"<<index<<":"<<ob_ptr<<" \t\tRef:"<<cursor->obRefcnt<<std::endl;
                cursor=cursor->next;
                ++index;
            }
            std::cout<<"\t<<<<<<<<<<<<<<<<<<<<"<<std::endl;
        }
    }
}