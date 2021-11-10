#ifndef __EXGC_POOL_VISITOR_H__
#define __EXGC_POOL_VISITOR_H__

#include <stdint.h>
#include <initializer_list>

namespace ExGC
{
    class GCObject;
    class IGCReferenceType;

    class GCPoolVisitor
    {
        uint8_t visitStrategy;
        uint8_t visitGenId;
        void doCalExtRefCnt(GCObject *);
        void doTraceReachable(GCObject *);
        
    public:
        enum VisitStrategy{CalExtRefCnt=0, TraceReachable};
        GCPoolVisitor(uint8_t strategy,uint8_t genId):visitStrategy(strategy),visitGenId(genId){};
        
        void _visit(GCObject *);
        void Visit(IGCReferenceType&);
    };
}

#endif