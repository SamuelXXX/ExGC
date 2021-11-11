#ifndef __EXGC_CORE_H__
#define __EXGC_CORE_H__

#include<stdint.h>
#include"ExGCPool.h"

#define InvalidGenID 99
#define WildGenID    100

namespace ExGC
{
    struct GCPoolHeader;
    class GCObject;

    class GCCore final 
    {
        bool m_enableRefCounter;
        bool m_enableAutoCollect;
        GCPool m_wildPool;
        GCPool m_generations[3];

    private:
        GCCore();
        GCCore(const GCCore &) = delete;
        void _collectPool(uint8_t);  // Collect target generation pool
        void _transferPool(uint8_t,uint8_t); // Transfer all nodes of one generation pool to another
        bool _shouldCollect(uint8_t); // Check if one pool's size exceed collecting threshold

    public:
        static GCCore *GetInstance();
        
        void Collect(uint8_t);
        void GCIncRef(GCObject *);
        void GCDecRef(GCObject *);
        void *Malloc(size_t);
        void Free(void *);
        void SetCollectThreshold(size_t);

        void CollectWhenFull(); // Collect when Generation is Full, Normally performed by auto collect process
        void EnableAutoCollect();
        void DisableAutoCollect();

        
    public: // Profiling Interface
        void MemoryProfile(); // Profiling Memory Occupation
        void GenerationProfile(int index); // Profiling Generation
        size_t GetGenerationSize(uint8_t genId);
        size_t GetGenerationMemory(uint8_t genId);
        size_t GetTotalSize();
        size_t GetTotalMemory();

        friend class GCObject;
    };
}

#endif