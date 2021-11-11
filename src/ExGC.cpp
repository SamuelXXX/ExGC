#include"ExGC.h"

namespace ExGC
{
    void ProfileGeneration(int gen_index)
    {
        GCCore::GetInstance()->MemoryProfile();
        GCCore::GetInstance()->GenerationProfile(gen_index);
    }

    void Profile()
    {
        GCCore::GetInstance()->MemoryProfile();
        GCCore::GetInstance()->GenerationProfile(0);
        GCCore::GetInstance()->GenerationProfile(1);
        GCCore::GetInstance()->GenerationProfile(2);
    }

    void CollectGeneration(int gen_index)
    {
        GCCore::GetInstance()->Collect(gen_index);
    }

    void Collect()
    {
        GCCore::GetInstance()->Collect(2);
    }
}