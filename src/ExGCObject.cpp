#include "ExGC.h"

namespace exgc
{
    void GCObject::GCIncRef(GCObject *ptr)
    {
        ++ptr->m_refcnt;
    }

    void GCObject::GCDecRef(GCObject *ptr)
    {
        if (--ptr->m_refcnt == 0)
        {
            ptr->m_header->owner->RemoveObject(ptr);
            delete ptr;
        }
            
    }

    void GCObject::TraceReference(GCPoolVisitor& v)
    {

    }

    void *GCObject::operator new(std::size_t size)
    {
        GCObject *ptr=(GCObject *)std::malloc(size);

        if(ptr!=nullptr)
            GCGenerationManager::GetInstance()->Add(ptr); // Add this pointer to maintaining area
        
        return ptr;
    }

    void GCObject::operator delete(void *ptr)
    {
        GCGenerationManager::GetInstance()->Remove((GCObject *)ptr); // Add this pointer from maintaining area
        std::free(ptr);
    }
}