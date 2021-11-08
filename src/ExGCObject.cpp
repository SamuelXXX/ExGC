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
        void *ptr=std::malloc(size+sizeof(size_t));
        *((size_t *)ptr)=size;

        GCObject *gc_ptr=(GCObject *)((char *)ptr+sizeof(size_t));
        GCGenerationManager::GetInstance()->m_objectsmem+=size;
        return gc_ptr;
    }

    void GCObject::operator delete(void *ptr) // Safety gurantee
    {
        GCObject *gc_ptr=(GCObject *)ptr;
        assert(gc_ptr->m_refcnt==0);
        
        // Back to real allocated pointer
        ptr=(char *)ptr-sizeof(size_t);
        GCGenerationManager::GetInstance()->m_objectsmem-=*((size_t *)ptr);
        std::free(ptr);
    }
}