#include"ExGC.h"

namespace exgc
{
    void Profile(int gen_index)
    {
        GCCore::GetInstance()->MemoryProfile();
        GCCore::GetInstance()->GenerationProfile(gen_index);
    }

    void Collect(int gen_index)
    {
        GCCore::GetInstance()->Collect(gen_index);
    }
}