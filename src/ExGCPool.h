#ifndef __EXGC_POOL_H__
#define __EXGC_POOL_H__
#include<stdint.h>

namespace exgc
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
        
        GCPoolHeader *head;
        GCPoolHeader *tail;
        
        size_t m_currentSize; // Current size of link
        size_t m_currentMemory; // Current allocated GCObject memory size
    
    private: // Node Management
        GCPoolHeader *addNode(GCPoolHeader *); // Insert a header node to pool, return added node
        GCPoolHeader *delNode(GCPoolHeader *); // Delete a header node to pool, return next node of deleted node
        
        GCPoolHeader *clearNodes(); // Clear all nodes and return header node pointer
        GCPoolHeader *linkNodes(GCPoolHeader *); // Link another nodes and return tail node pointer
    
    private:
        void CollectPool(); // Collect all cycle reference inside this pool
        bool ShouldGC(); // Current size exceed m_maxSize

    private:
        GCPool() = delete;
        GCPool(uint8_t, size_t);

    public:
        size_t GetGCObjectMemory(); // Get allocated GCObjects mem size
        size_t CalcGCObjectMemory(); // Iterating all GCObjects and calculate memory size
        size_t GetSize(); // Current amount of GCObject allocated in this
        size_t CalcSize(); // Iterating all GCObjects and calculate link list size
        bool Contain(GCPoolHeader *); // Check if header in this generation
        
        
    
    private: // Profile interfaces
        void Profile();
    
    friend class GCCore;
    };
}

#endif