#include "ExGCPool.h"

namespace exgc
{
    ExGCGenerationPool generation1(1024);

    ExGCGenerationPool::ExGCGenerationPool(uint32_t size)
    {
        head = new GCPoolHeader[size];
        tail = head + size;
        current = head;
    }

    void ExGCGenerationPool::AddItem(GCPoolItemBase *item)
    {
        current->m_item = item;
        current->owner = this;
        current->temp_refcnt = 0;

        item->m_header = current;
        current++;
    }

    void ExGCGenerationPool::DestroyItem(GCPoolItemBase *item)
    {
        GCPoolHeader *item_header = item->m_header;
        // In this pool and no more reference found
        if (item_header >= head && item_header < tail && item->m_refcnt == 0)
        {
            delete item;
            --current;
            if (item_header != current)
                memcpy(item_header, current, sizeof(GCPoolHeader));
        }
    }

    void ExGCGenerationPool::Collect()
    {
        
    }

    inline void GCIncRef(GCPoolItemBase *ptr)
    {
        ++ptr->m_refcnt;
    }

    inline void GCDecRef(GCPoolItemBase *ptr)
    {
        if (--ptr->m_refcnt == 0)
            ptr->m_header->owner->DestroyItem(ptr);
    }
}