#include "ExGC.h"
#include<iostream>
#include<string>

namespace exgc
{
    GCPoolManager::GCPoolManager(uint32_t size) 
    {
        head = new GCPoolHeader[size];
        tail = head + size;
        current = head;
    }

    void GCPoolManager::Push(GCObject *item)
    {
        current->m_item = item;
        current->owner = this;
        current->temp_refcnt = 0;

        item->m_header = current;
        current++;
    }

    void GCPoolManager::Kick(GCObject *item)
    {
        
        GCPoolHeader *item_header = item->m_header;
        assert(Contain(item_header));

        // In this pool and no more reference found
        --current;
        if (item_header != current)
        {
            transfer(item_header,current);
        }
    }

    GCObject *GCPoolManager::Pop()
    {
        if(current==head)
        {
            return nullptr;
        }
        --current;
        return current->m_item;
    }

    void GCPoolManager::CollectPool()
    {
        GCPoolHeader *header;
        std::size_t prev_size=Size();
        for(header=head;header!=current;++header)
        {
            header->temp_refcnt=header->m_item->m_refcnt;
        }

        for(header=head;header!=current;++header)
        {
            header->m_item->GCTrackReference();
        }

        for(header=head;;)
        {
            while (header->temp_refcnt==0&&header!=current)
            {
                GCObject *obj=header->m_item;
                Kick(obj);
                obj->m_refcnt=0;
                delete obj;
            }
            if(header==current)
                break;
            ++header;
        }
        std::size_t size_changed=prev_size-Size();
        GCLog(nullptr, std::to_string(size_changed)+" Objects is Collected!");
    }

    void GCPoolManager::DebugLog()
    {
        std::cout<<std::endl;
        GCLog(nullptr, "Begin Pool DebugLog ##################################################");
        std::cout<<"TotalMemory:"<<TotalMemory()<<std::endl;
        std::cout<<"HeaderPoolMemory:"<<HeaderPoolMemory()<<std::endl;
        std::cout<<"Capacity:"<<Capacity()<<std::endl;
        std::cout<<"Size:"<<Size()<<std::endl;
        std::cout<<"ReservedSize:"<<ReservedSize()<<std::endl;
        std::cout<<"IsFull:"<<IsFull()<<std::endl;
        std::cout<<"IsEmpty:"<<IsEmpty()<<std::endl;
        std::cout<<">>>>>>>>>>Managed Objects<<<<<<<<<<"<<std::endl;
        for(auto header=head;header!=current;++header)
        {
            std::cout<<"Index: "<<static_cast<size_t>(header-head)<<"\t\tAddress:"<<header->m_item<<" \t\tRefCount:"<<header->m_item->m_refcnt<<std::endl;
        }
        std::cout<<">>>>>>>>>>End<<<<<<<<<<"<<std::endl;
        GCLog(nullptr, "Finish Pool DebugLog ##################################################");
        std::cout<<std::endl;
    }
}