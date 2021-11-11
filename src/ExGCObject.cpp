#include "ExGC.h"
#include<assert.h>

namespace ExGC
{
    void GCObject::GCTrackReference(GCPoolVisitor& v)
    {
        throw "GCTrackReference method not implemented!!!";
    }

    void *GCObject::operator new(std::size_t size)
    {
        return GCCore::GetInstance()->Malloc(size);
    }

    void GCObject::operator delete(void *ptr)
    {
        GCCore::GetInstance()->Free(ptr);
    }
}