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
                obj->m_refcnt=0; // Set reference to 0 before deleting GCObject
                delete obj;
            }
            if(header==current)
                break;
            ++header;
        }
        std::size_t size_changed=prev_size-Size();
        GCLog(nullptr, std::to_string(size_changed)+" Objects are Collected!");
    }

    void GCPoolManager::Profile()
    {
        std::cout<<"\tCapacity:"<<Capacity()<<std::endl;
        std::cout<<"\tSize:"<<Size()<<std::endl;
        std::cout<<"\tReservedSize:"<<ReservedSize()<<std::endl;
        if(!IsEmpty())
        {
            std::cout<<"\t>>>>>>>>>>>>>>>>>>>>"<<std::endl;
            for(auto header=head;header!=current;++header)
            {
                std::cout<<"\t\t"<<static_cast<size_t>(header-head)<<":"<<header->m_item<<" \t\tRef:"<<header->m_item->m_refcnt<<std::endl;
            }
            std::cout<<"\t<<<<<<<<<<<<<<<<<<<<"<<std::endl;
        }
    }
}