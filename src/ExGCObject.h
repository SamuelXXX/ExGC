#ifndef __EXGC_OBJECT_H__
#define __EXGC_OBJECT_H__

#include<stdint.h>

namespace exgc
{
    class GCPoolVisitor;
    class GCPool;

    class GCObject // ***Defination Complete
    {
        uint32_t m_refcnt;

    public:
        void *operator new(size_t);
        void operator delete(void *ptr); // Call destructor, kick from pool and free space
        

    public:
        virtual void GCTrackReference(GCPoolVisitor&);

    public:
        void IncRef();
        void DecRef();
        void ResetRef();
        uint32_t GetRefCount();

    public:
        GCObject()=default; //Do Nothing
        virtual ~GCObject()=default;

        friend class GCPool;
    };
}

#endif