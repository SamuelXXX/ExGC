#ifndef __EXGC_POOL_VISITOR_H__
#define __EXGC_POOL_VISITOR_H__

#include <stdint.h>

namespace exgc
{
    class GCObject;
    template <class T>
    class Ref;

    class GCPoolVisitor
    {
        uint8_t visitStrategy;
        uint8_t visitGenId;
        void doTraceDirectRef(GCObject *);
        void doTraceIndirectRef(GCObject *);
        
    public:
        enum VisitStrategy{TraceDirectRef=0, TraceIndirectRef};
        GCPoolVisitor(uint8_t strategy,uint8_t genId):visitStrategy(strategy),visitGenId(genId){};
        
        void Visit(GCObject *);

        template<class T>
        void Visit(const Ref<T>& ptr_ref)
        {
            this->Visit(ptr_ref.m_object_ptr);
        }
    };
}

#endif