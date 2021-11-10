#include"ExGC.h"

namespace exgc
{
    void GCPoolVisitor::_visit(GCObject *ob_ptr)
    {
        if(ob_ptr==nullptr)
            return;
        switch(visitStrategy)
        {
            case TraceDirectRef:doTraceDirectRef(ob_ptr);break;
            case TraceIndirectRef:doTraceIndirectRef(ob_ptr);break;
            default:break;
        }
    }

    void GCPoolVisitor::Visit(IGCReferenceType& ref)
    {
        ref.OnRefVisited(*this);
    }

    void GCPoolVisitor::doTraceDirectRef(GCObject *ob_ptr)
    {
        GCPoolHeader *headerPtr=ExTractHeaderPtr(ob_ptr);
        if(headerPtr->obGenId!=visitGenId) // Not in visiting target generation
            return;
        
        --headerPtr->extRefcnt; // Reduce external reference count
    }

    void GCPoolVisitor::doTraceIndirectRef(GCObject *ob_ptr)
    {
        GCPoolHeader *headerPtr=ExTractHeaderPtr(ob_ptr);
        if(headerPtr->obGenId!=visitGenId) // Not in visiting target generation
            return;

        if(headerPtr->trackState.reachable) // this is a marked node
            return;
        
        headerPtr->trackState.reachable=true; // Mark as a reachable object
        ob_ptr->GCTrackReference(*this);// Recursive call track reference
    }
}