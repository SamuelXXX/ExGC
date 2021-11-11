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
        GCPool m_wildPool;
        GCPool m_generations[3];

    private:
        GCCore();
        GCCore(const GCCore &) = delete;
        void _collectPool(uint8_t);
        void _transferPool(uint8_t,uint8_t);
        bool _poolOversized(uint8_t);
        void _recursiveCollect(uint8_t);

    public:
        static GCCore *GetInstance();
        void Collect(uint8_t);
        void GCIncRef(GCObject *);
        void GCDecRef(GCObject *);
        void *Malloc(size_t);
        void Free(void *);
        
    public: // Profiling Interface
        void MemoryProfile(); // Profiling Memory Occupation
        void GenerationProfile(int index); // Profiling Generation
        size_t GetGenerationSize(uint8_t genId);
        size_t GetGenerationMemory(uint8_t genId);

        friend class GCObject;
    };
}

#endif