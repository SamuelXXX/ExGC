#include "ExGC.h"

namespace exgc
{
    GCPoolManager::GCPoolManager(uint32_t size) : m_visitor(this)
    {
        head = new GCPoolHeader[size];
        tail = head + size;
        current = head;
    }

    void GCPoolManager::AddObject(GCObject *item)
    {
        current->m_item = item;
        current->owner = this;
        current->temp_refcnt = 0;

        item->m_header = current;
        current++;
    }

    void GCPoolManager::RemoveObject(GCObject *item)
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

    void GCPoolManager::Collect()
    {
        
    }
}