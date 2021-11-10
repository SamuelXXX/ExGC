#include "ExGC.h"
#include<assert.h>

namespace exgc
{
    void GCObject::IncRef()
    {
        ++m_refcnt;
    }

    void GCObject::DecRef()
    {
        --m_refcnt;
    }

    void GCObject::ResetRef()
    {
        m_refcnt=0;
    }

    uint32_t GCObject::GetRefCount()
    {
        return m_refcnt;
    }

    void GCObject::GCTrackReference(GCPoolVisitor& visitor)
    {
        throw "GCTrackReference method not implemented!!!";
    }

    void *GCObject::operator new(std::size_t size)
    {
        // Assign space and make connection to pool
        void *ptr=std::malloc(size+sizeof(GCPoolHeader));
        GCPoolHeader *header_ptr=(GCPoolHeader *)ptr;
        GCObject *ob_ptr=(GCObject *)((uint8_t *)ptr+sizeof(GCPoolHeader));

        header_ptr->obSize=size;
        header_ptr->obGenId=InvalidGenID;

        GCCore::GetInstance()->makeWild(header_ptr);

        return ob_ptr;
    }

    void GCObject::operator delete(void *ptr) // Call destructor, kick from pool and free space
    {
        GCObject *ob_ptr=(GCObject *)ptr;
        GCPoolHeader *header_ptr=(GCPoolHeader *)((uint8_t *)ptr-sizeof(GCPoolHeader));
        assert(ob_ptr->m_refcnt<=0);
        
        if(header_ptr->obGenId!=InvalidGenID) // Still in pool
            GCCore::GetInstance()->kick(header_ptr);

        std::free(header_ptr);
    }
}