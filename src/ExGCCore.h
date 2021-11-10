#ifndef __EXGC_CORE_H__
#define __EXGC_CORE_H__

#include<stdint.h>
#include"ExGCPool.h"

namespace ExGC
{
    struct GCPoolHeader;
    class GCObject;

    class GCCore final 
    {
        bool m_refCounterFlag;
        GCPool m_wild;
        GCPool m_gen1;
        GCPool m_gen2;
        GCPool m_gen3;

    private:
        GCCore();
        GCCore(const GCCore &) = delete;
        void makeWild(GCPoolHeader *); // Make target allocated object a wild pointer
        void makeManaged(GCPoolHeader *); // Make a wild pointer managed by gc
        void ascend(GCPoolHeader *); // Increase generation of target object
        void kick(GCPoolHeader *); // Kick target object from all generation pool

    public:
        static GCCore *GetInstance();
        void Collect(int);
        void GCIncRef(GCObject *);
        void GCDecRef(GCObject *);
        void ToggleReferenceCounter(bool flag);
        
    public: // Profiling Interface
        void MemoryProfile(); // Profiling Memory Occupation
        void GenerationProfile(int index); // Profiling Generation
        size_t GetGenerationSize(uint8_t genId);
        size_t GetGenerationMemory(uint8_t genId);

        friend class GCObject;
    };
}

#endif