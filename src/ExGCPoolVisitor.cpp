#include"ExGC.h"

namespace ExGC
{
    void GCPoolVisitor::_visit(GCObject *ob_ptr)
    {
        if(ob_ptr==nullptr)
            return;
        switch(visitStrategy)
        {
            case CalExtRefCnt:doCalExtRefCnt(ob_ptr);break;
            case TraceReachable:doTraceReachable(ob_ptr);break;
            default:break;
        }
    }

    void GCPoolVisitor::Visit(IGCReferenceType& ref)
    {
        ref.OnRefVisited(*this);
    }

    void GCPoolVisitor::doCalExtRefCnt(GCObject *ob_ptr)
    {
        GCPoolHeader *headerPtr=ExTractHeaderPtr(ob_ptr);
        if(headerPtr->obGenId!=visitGenId) // Not in target generation
            return;
        
        --headerPtr->extRefcnt; // Reduce external reference count
    }

    void GCPoolVisitor::doTraceReachable(GCObject *ob_ptr)
    {
        GCPoolHeader *headerPtr=ExTractHeaderPtr(ob_ptr);
        if(headerPtr->obGenId!=visitGenId) // Not in target generation
            return;

        if(headerPtr->trackState.reachable) // this is a marked node
            return;
        
        headerPtr->trackState.reachable=true; // Mark as a reachable object
        ob_ptr->GCTrackReference(*this);// Recursive track reachable
    }
}