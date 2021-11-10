#ifndef __EXGC_OBJECT_H__
#define __EXGC_OBJECT_H__

#include<stdint.h>

#define BeginCaptureRefWithBase(ClassName,BaseClassName) void ClassName::GCTrackReference(GCPoolVisitor& v):BaseClassName::GCTrackReference(v){
#define BeginCaptureRef(ClassName) void ClassName::GCTrackReference(GCPoolVisitor& v){
#define CaptureRef(var) v.Visit(var);
#define EndCaptureRef }

namespace ExGC
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