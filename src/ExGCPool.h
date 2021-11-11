#ifndef __EXGC_POOL_H__
#define __EXGC_POOL_H__
#include<stdint.h>

namespace ExGC
{
    class GCCore;

    struct TrackState
    {
        bool trackRoot;
        bool reachable;
    };

    struct GCPoolHeader // ***Defination Complete
    {
        uint8_t obGenId;
        GCPoolHeader *prev;
        GCPoolHeader *next;
        
        size_t obSize;
        uint32_t obRefcnt;
        union 
        {
            uint32_t extRefcnt;
            TrackState trackState;
        };
    };
    
    class GCPool final
    {
        const uint8_t m_genId;
        size_t m_collectThreshold;
        size_t m_transferTimes;
        
        GCPoolHeader *head;
        GCPoolHeader *tail;
        
        size_t m_currentSize; // Current size of link
        size_t m_currentMemory; // Current allocated GCObject memory size

        GCPool() = delete;
        GCPool(uint8_t, size_t);
    
    private: // Node Management
        GCPoolHeader *addNode(GCPoolHeader *); // Append a header node to pool, return added node
        GCPoolHeader *delNode(GCPoolHeader *); // Delete a header node to pool, return next node of deleted node
        void transfer(GCPool& other); // Transfer all of my nodes to another pool
        GCPoolHeader *_discardAllNodes(); // Clear all my nodes and return header node pointer
        GCPoolHeader *_adoptAllNodes(GCPoolHeader *); // Adopt another link list and return tail node pointer

    public:
        size_t GetGCObjectMemory(); // Get allocated GCObjects mem size
        size_t CalcGCObjectMemory(); // Iterating all GCObjects and calculate memory size
        size_t GetSize(); // Current amount of GCObject allocated in this
        size_t CalcSize(); // Iterating all GCObjects and calculate link list size
        
    private: // Profile interfaces
        void Profile();
    
    friend class GCCore;
    };
}

#endif