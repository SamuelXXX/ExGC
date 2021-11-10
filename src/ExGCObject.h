#ifndef __EXGC_OBJECT_H__
#define __EXGC_OBJECT_H__

#include<stdint.h>

namespace exgc
{
    class GCPoolVisitor;
    class GCObject // ***Defination Complete
    {
    public:
        void *operator new(size_t);
        void operator delete(void *ptr); // Call destructor, kick from pool and free space
        // Destructor must be declared as virtual, otherwise sub class of GCObject will not release 'Ref<T>' property as expected
        virtual ~GCObject()=default; 
        
    public:
        virtual void GCTrackReference(GCPoolVisitor&)=0;
    };
}

#endif