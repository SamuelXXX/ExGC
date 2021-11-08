#include "ExGC.h"
#include<iostream>

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
        std::cout<<size_changed<<" Objects is Collected!"<<std::endl;
    }
}