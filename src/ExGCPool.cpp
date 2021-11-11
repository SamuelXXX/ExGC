#include "ExGC.h"
#include<iostream>
#include<string>
#include<assert.h>

namespace ExGC
{
    GCPool::GCPool(uint8_t genId,size_t maxSize):
    m_genId(genId),
    m_collectThreshold(maxSize),
    head(nullptr),
    tail(nullptr),
    m_currentSize(0),
    m_currentMemory(0),
    m_transferTimes(0)
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

    void GCPool::transfer(GCPool& other)
    {
        other._adoptAllNodes(this->_discardAllNodes());
    }

    GCPoolHeader *GCPool::_discardAllNodes()
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

    GCPoolHeader *GCPool::_adoptAllNodes(GCPoolHeader *anotherHeader)
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