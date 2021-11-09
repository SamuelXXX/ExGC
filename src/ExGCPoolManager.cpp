#include "ExGC.h"
#include<iostream>
#include<string>

namespace exgc
{
    GCPoolManager::GCPoolManager(uint8_t genId,size_t maxSize):
    m_genId(genId),
    m_maxSize(maxSize),
    head(nullptr),
    tail(nullptr),
    m_currentSize(0),
    m_currentMemory(0)
    {

    }

    GCPoolHeader *GCPoolManager::addNode(GCPoolHeader *newNode)
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

    GCPoolHeader *GCPoolManager::delNode(GCPoolHeader *nodeDelete)
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

    size_t GCPoolManager::GetGCObjectMemory()
    {
        return m_currentMemory;
    }

    size_t GCPoolManager::GetSize()
    {
        return m_currentSize;
    }

    bool GCPoolManager::Contain(GCPoolHeader *node)
    {
        return node->obGenId==m_genId;
    }

    bool GCPoolManager::ShouldGC()
    {
        return m_currentSize>=m_maxSize;
    }

    void GCPoolManager::CollectPool()
    {
        if(m_currentSize==0)
            return;

        size_t sizeBeforeCollect=m_currentSize;

        // Initialize all external reference count value
        GCPoolHeader *cursorPtr=head;
        while (cursorPtr)
        {
            GCObject *obPtr=ExTractObjectPtr(cursorPtr);
            cursorPtr->extRefcnt=obPtr->GetRefCount();
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
        // Disable auto inc-dec refcnt, or delete operation may cause chain reaction of destructing GCObjects
        GCGenerationManager::GetInstance()->ToggleReferenceCounter(false); 
        cursorPtr=head;
        while (cursorPtr)
        {
            GCObject *obPtr=ExTractObjectPtr(cursorPtr);
            GCPoolHeader *nextPtr=cursorPtr->next;

            if(!cursorPtr->trackState.reachable)
            {
                delNode(cursorPtr);
                obPtr->ResetRef(); // Reset reference before delete target GCObject
                delete obPtr;
            }

            cursorPtr=nextPtr;
        }
        GCGenerationManager::GetInstance()->ToggleReferenceCounter(true); // Resume auto inc-dec refcnt

        size_t sizeCollected=sizeBeforeCollect-m_currentSize;
        GCLog(nullptr,std::to_string(sizeCollected)+" Objects Collected in Generation "+std::to_string(m_genId));        
    }

    void GCPoolManager::Profile()
    {
        std::cout<<"\tMaxSize:"<<m_maxSize<<std::endl;
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
                std::cout<<"\t\t"<<index<<":"<<ob_ptr<<" \t\tRef:"<<ob_ptr->GetRefCount()<<std::endl;
                cursor=cursor->next;
                ++index;
            }
            std::cout<<"\t<<<<<<<<<<<<<<<<<<<<"<<std::endl;
        }
    }
}