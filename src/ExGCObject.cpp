#include "ExGC.h"
#include<string>

namespace exgc
{
    void GCObject::GCIncRef(GCObject *ptr)
    {
        if(ptr->m_header==nullptr) // Not managed yet!
            GCGenerationManager::GetInstance()->add(ptr); // Add this pointer to maintaining pool
        ++ptr->m_refcnt;
    }

    void GCObject::GCDecRef(GCObject *ptr)
    {
        if (--ptr->m_refcnt == 0)
        {
            GCLog(ptr, "Kill GCObject due to reference reduced to 0");
            ptr->m_header->owner->Kick(ptr); // Kick self out from pool;
            delete ptr;// Safely free this object
        } 
    }

    void GCObject::GCTrackReference()
    {
        throw "GCTrackReference method not implemented!!!";
    }

    void *GCObject::operator new(std::size_t size)
    {
        // Assign space and make connection to pool
        GCObject *ptr=(GCObject *)std::malloc(size);
        return ptr;
    }

    void GCObject::operator delete(void *ptr) // Safety gurantee
    {
        GCObject *gc_ptr=(GCObject *)ptr;
        assert(gc_ptr->m_refcnt==0);
        std::free(ptr);
    }
}